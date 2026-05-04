#include "parameters.h"
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

bool Parameters::is_blank(const std::string& string) {
    for (char c : string) {
        if (c == '#') return true;
        if(!isspace(c)) return false;
    }
    return true;
}
void Parameters::parse(std::istream& content, unsigned int offset_in_file) {
    std::string line;
    int l_count = offset_in_file;
    while (std::getline(content, line))
    {
        l_count++;
        if (is_blank(line) || line[0] == '#') continue;
        
        std::istringstream stream(line);

        std::string type;
        std::string key;
        std::string equal_sign;
        if (!(stream >> type >> key >> equal_sign)) print_error(line, l_count, "Error of type or key !");
        if (equal_sign != "=") print_error(line, l_count, "Expected \033[0m\033[31m=\033[1m, got \033[0m\033[31m" + equal_sign + "\033[1m.");

        if (type == "int" || type == "float") {
            float value;
            if (stream >> value) float_vars[key] = value;
            else print_error(line, l_count, type + " parameter " + key + " has no value.");
        }
        else if (type == "bool") {
            std::string value;
            if (stream >> value) {
                if (value == "false" || value == "true") float_vars[key] = (value == "false" ? false : true);
                else print_error(line, l_count, "bool parameter " + key + " should be \033[0m\033[31mtrue\033[1m of \033[0m\033[31mfalse\033[1m, got \033[0m\033[31m" + value + "\033[1m instead.");
            }
            else print_error(line, l_count, "bool parameter " + key + " has no value.");
        }
        else if (type == "str") {
            std::string value;
            if (stream >> value) str_vars[key] = value;
            else print_error(line, l_count, "string parameter " + key + " has no value");
        }
    }
}
void Parameters::apply_replacements(const std::string& content, unsigned int offset_in_file, const std::string& path, std::vector<Parameters>& output, std::list<std::pair<std::string, std::list<std::string>>>::iterator replacements, std::list<std::pair<std::string, std::list<std::string>>>::iterator end) {
    std::pair<std::string, std::list<std::string>> pair = *replacements;
    replacements++;
    std::string key = pair.first;

    for (std::string replacement : pair.second) {
        std::string modified_all = std::regex_replace(content, std::regex(key), replacement);

        if (replacements == end) {
            std::istringstream modified_content(modified_all);
            output.push_back(Parameters(modified_content, offset_in_file, path));
        }
        else apply_replacements(modified_all, offset_in_file, path, output, replacements, end);
    }
}
void Parameters::parse_with_iterators(std::istream& content, unsigned int offset_in_file, const std::string& path, std::vector<Parameters>& output) {
    std::string all = "";
    std::string line;
    std::list<std::pair<std::string, std::list<std::string>>> replacements = std::list<std::pair<std::string, std::list<std::string>>>();
    int l_count = offset_in_file;
    while (std::getline(content, line))
    {
        l_count++;
        if (is_blank(line)) {
            all += line + "\n";
            continue;
        }
        
        std::istringstream stream(line);

        std::string first_word;
        stream >> first_word;

        if (first_word == "iterate") {
            std::string key;
            stream >> key;
            if (key[0] != '<' || key[key.size()-1] != '>') print_error(line, l_count, path, "Iterate is not followed by a key surrounded by <> !");

            std::list<std::string> replacement = std::list<std::string>();
            std::string word;
            stream >> word;
            while (word != "<end>")
            {
                replacement.push_back(word);
                if (!(stream >> word)) print_error(line, l_count, path, "Iterate does not end with key <end> !");
            }
            replacements.push_back(std::pair<std::string, std::list<std::string>>(key, replacement));

            all += "# " + line + "\n";
        }
        else all += line + "\n";
    }

    if (replacements.size() == 0) {
        std::istringstream modified_content(all);
        output.push_back(Parameters(modified_content, offset_in_file, path));
    }
    else apply_replacements(all, offset_in_file, path, output, replacements.begin(), replacements.end());
}

void Parameters::print_error(const std::string& line, unsigned int line_index, const std::string& path, const std::string& message) {
    std::cerr << "\033[1;31m";
    std::cerr << "| ERROR: line " << line_index << " of content " << path << ":\n";
    std::cerr << "| \t" << line << "\n";
    std::cerr << "| " << message << "\033[0m\n";
    exit(1);
}
void Parameters::print_error(const std::string& line, unsigned int line_index, const std::string& message) const {
    print_error(line, line_index, this->path, message);
}

Parameters::Parameters(const std::string& path): path(path) {}
Parameters::Parameters(std::istream& content, unsigned int offset_in_file, const std::string& path): Parameters(path) {
    this->parse(content, offset_in_file);
}

bool Parameters::get_bool(const std::string& key) const {
    if (!contains_bool(key)) {
        std::cerr << "\033[1;31mERROR: File \"" << path << "\" has no value for key " << key << "!\033[0m\n";
        exit(1);
    }
    return float_vars.at(key);
}
int Parameters::get_int(const std::string& key) const {
    if (!contains_int(key)) {
        std::cerr << "\033[1;31mERROR: File \"" << path << "\" has no value for key " << key << "!\033[0m\n";
        exit(1);
    }
    return float_vars.at(key);
}
unsigned int Parameters::get_uint(const std::string& key) const {
    return get_int(key);
}
float Parameters::get_float(const std::string& key) const {
    if (!contains_float(key)) {
        std::cerr << "\033[1;31mERROR: File \"" << path << "\" has no value for key " << key << "!\033[0m\n";
        exit(1);
    }
    return float_vars.at(key);
}
const std::string& Parameters::get_string(const std::string& key) const {
    if (!contains_string(key)) {
        std::cerr << "\033[1;31mERROR: File \"" << path << "\" has no value for key " << key << "!\033[0m\n";
        exit(1);
    }
    return str_vars.at(key);
}

bool Parameters::get_bool(const std::string& key, bool default_value) const {
    if (!contains_bool(key)) return default_value;
    return float_vars.at(key);
}
int Parameters::get_int(const std::string& key, int default_value) const {
    if (!contains_int(key)) return default_value;
    return float_vars.at(key);
}
unsigned int Parameters::get_uint(const std::string& key, int default_value) const {
    return get_int(key, default_value);
}
float Parameters::get_float(const std::string& key, float default_value) const {
    if (!contains_float(key)) return default_value;
    return float_vars.at(key);
}
const std::string& Parameters::get_string(const std::string& key, const std::string& default_value) const {
    if (!contains_string(key)) return default_value;
    return str_vars.at(key);
}

bool Parameters::contains_bool(const std::string& key) const {
    return float_vars.count(key);
}
bool Parameters::contains_int(const std::string& key) const {
    return float_vars.count(key);
}
bool Parameters::contains_uint(const std::string& key) const {
    return contains_int(key);
}
bool Parameters::contains_float(const std::string& key) const {
    return float_vars.count(key);
}
bool Parameters::contains_string(const std::string& key) const {
    return str_vars.count(key);
}

std::ostream& Parameters::cout(std::ostream& c) const {
    for (const auto& [key, value] : this->str_vars)
        c << key << " = " << value << "\n";
    for (const auto& [key, value] : this->float_vars)
        c << key << " = " << value << "\n";
    return c;
}
std::ostream& operator<<(std::ostream& c, const Parameters& p) {
    return p.cout(c);
}
