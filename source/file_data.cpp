#include "file_data.h"
#include <sstream>
#include <fstream>
#include <regex>
#include <filesystem>

FileData::FileData(std::istream& content, const std::string& path): path(path) {
    std::string line;
    int l_count = 0;
    while (std::getline(content, line))
    {
        l_count++;
        if (line.size() == 0 || line[0] == '#') continue;
        
        std::istringstream stream(line);

        std::string type;
        std::string key;
        std::string equal_sign;
        if (!(stream >> type >> key >> equal_sign)) {
            std::cerr << "\033[1;31mERROR: line " << l_count << " of content " << path << " contains an error of type or key\033[0m\n";
            exit(1);
        }
        if (equal_sign != "=") {
            std::cerr << "\033[1;31mERROR: expected \033[0m\033[31m=\033[1m, got \033[0m\033[31m" << equal_sign << "\033[1m.\nLine " << l_count << " of content " << path << "\033[0m\n";
            exit(1);
        }

        if (type == "int" || type == "float") {
            float value;
            if (stream >> value) float_vars[key] = value;
            else {
                std::cerr << "\033[1;31mERROR: " << type << " parameter " << key << " has no value.\nLine " << l_count << " of content " << path << "\033[0m\n";
                exit(1);
            }
        }
        else if (type == "bool") {
            std::string value;
            if (stream >> value) {
                if (value == "false" || value == "true") float_vars[key] = (value == "false" ? false : true);
                else {
                    std::cerr << "\033[1;31mERROR: bool parameter " << key << " should be \033[0m\033[31mtrue\033[1m of \033[0m\033[31mfalse\033[1m, got \033[0m\033[31m" << value << "\033[1m instead.\nLine " << l_count << " of content " << path << "\033[0m\n";
                    exit(1);
                }
            }
            else {
                std::cerr << "\033[1;31mERROR: bool parameter " << key << " has no value.\nLine " << l_count << " of content " << path << "\033[0m\n";
                exit(1);
            }
        }
        else if (type == "str") {
            std::string value;
            if (stream >> value) str_vars[key] = value;
            else {
                std::cerr << "\033[1;31mERROR: string parameter " << key << " has no value.\nLine " << l_count << " of content " << path << "\033[0m\n";
                exit(1);
            }
        }
    }
}

void FileData::set_seed(unsigned int seed) { float_vars["seed"] = seed; }
void FileData::set_default_seed(unsigned int seed) { if (!contains_bool("seed")) float_vars["seed"] = seed; }
unsigned int FileData::get_seed() const  { return get_int("seed"); }

bool FileData::get_bool(const std::string& key) const {
    if (!contains_bool(key)) {
        std::cerr << "\033[1;31mERROR: File \"" << path << "\" has no value for key " << key << "!\033[0m\n";
        exit(1);
    }
    return float_vars.at(key);
}
int FileData::get_int(const std::string& key) const {
    if (!contains_int(key)) {
        std::cerr << "\033[1;31mERROR: File \"" << path << "\" has no value for key " << key << "!\033[0m\n";
        exit(1);
    }
    return float_vars.at(key);
}
float FileData::get_float(const std::string& key) const {
    if (!contains_float(key)) {
        std::cerr << "\033[1;31mERROR: File \"" << path << "\" has no value for key " << key << "!\033[0m\n";
        exit(1);
    }
    return float_vars.at(key);
}
const std::string& FileData::get_string(const std::string& key) const {
    if (!contains_string(key)) {
        std::cerr << "\033[1;31mERROR: File \"" << path << "\" has no value for key " << key << "!\033[0m\n";
        exit(1);
    }
    return str_vars.at(key);
}

bool FileData::get_bool(const std::string& key, bool default_value) const {
    if (!contains_bool(key)) return default_value;
    return float_vars.at(key);
}
int FileData::get_int(const std::string& key, int default_value) const {
    if (!contains_int(key)) return default_value;
    return float_vars.at(key);
}
float FileData::get_float(const std::string& key, float default_value) const {
    if (!contains_float(key)) return default_value;
    return float_vars.at(key);
}
const std::string& FileData::get_string(const std::string& key, const std::string& default_value) const {
    if (!contains_string(key)) return default_value;
    return str_vars.at(key);
}

bool FileData::contains_bool(const std::string& key) const {
    return float_vars.count(key);
}
bool FileData::contains_int(const std::string& key) const {
    return float_vars.count(key);
}
bool FileData::contains_float(const std::string& key) const {
    return float_vars.count(key);
}
bool FileData::contains_string(const std::string& key) const {
    return str_vars.count(key);
}

std::ostream& operator<<(std::ostream& c, const FileData& f) {
    for (const auto& [key, value] : f.str_vars)
        std::cout << key << " = " << value << "\n";
    for (const auto& [key, value] : f.float_vars)
        std::cout << key << " = " << value << "\n";
    return c << "\033[1A";
}

void apply_replacements(const std::string& content, const std::string& path, std::list<FileData>& result, std::list<std::pair<std::string, std::list<std::string>>>::iterator replacements, std::list<std::pair<std::string, std::list<std::string>>>::iterator end) {
    std::pair<std::string, std::list<std::string>> pair = *replacements;
    replacements++;
    std::string key = pair.first;

    for (std::string replacement : pair.second) {
        std::string modified_all = std::regex_replace(content, std::regex(key), replacement);

        if (replacements == end) {
            std::istringstream content(modified_all);
            result.push_back(FileData(content, path));
        }
        else apply_replacements(modified_all, path, result, replacements, end);
    }
}
std::list<FileData> generate_file_data(const std::string& path, bool allow_import) {
    std::list<FileData> result = std::list<FileData>();
    std::string directory = std::filesystem::path(path).parent_path().u8string();
    std::ifstream file = std::ifstream(path);
    if (!file.is_open()) {
        std::cerr << "\033[1;31mERROR: cannot open data_file at \"" << path << "\" !\033[0m\n";
        exit(1);
    }

    std::string all = "";
    std::string line;
    std::list<std::pair<std::string, std::list<std::string>>> replacements = std::list<std::pair<std::string, std::list<std::string>>>();
    int l_count = 0;
    while (std::getline(file, line))
    {
        l_count++;
        if (line.size() == 0 || line[0] == '#') {
            all += line + "\n";
            continue;
        }
        
        std::istringstream stream(line);

        std::string first_word;
        stream >> first_word;

        if (first_word == "iterate") {
            std::string key;
            stream >> key;
            if (key[0] != '<' || key[key.size()-1] != '>') {
                std::cerr << "\033[1;31mERROR: line " << l_count << " of content " << path << " iterate is not followed by a key surrounded by <>\033[0m\n";
                exit(1);
            }

            std::list<std::string> replacement = std::list<std::string>();
            std::string word;
            stream >> word;
            while (word != "<end>")
            {
                replacement.push_back(word);
                if (!(stream >> word)) {
                    std::cerr << "\033[1;31mERROR: line " << l_count << " of content " << path << " iterate does not end with key <end>\033[0m\n";
                    exit(1);
                }
            }
            replacements.push_back(std::pair<std::string, std::list<std::string>>(key, replacement));

            all += "# " + line + "\n";
        }
        else if (first_word == "import") {
            std::string import_path;
            stream >> import_path;

            if (allow_import) result.splice(result.end(), generate_file_data(directory + "/" + import_path, false));

            all += "# " + line + "\n";
        }
        else all += line + "\n";
    }


    if (replacements.size() == 0) {
        std::istringstream content(all);
        return std::list<FileData>{ FileData(content, path) };
    }

    apply_replacements(all, path, result, replacements.begin(), replacements.end());
    return result;
}
