#include "file_data.h"
#include <sstream>
#include <fstream>

FileData::FileData(const std::string& path): path(path) {
    std::ifstream file = std::ifstream(path);
    if (!file.is_open()) {
        std::cerr << "\033[1;31mERROR: cannot open peoples file at \"" << path << "\" !\033[0m\n";
        exit(1);
    }

    std::string line;
    int l_count = 0;
    while (std::getline(file, line))
    {
        l_count++;
        if (line.size() == 0 || line[0] == '#') continue;
        
        std::istringstream stream(line);

        std::string type;
        std::string key;
        std::string equal_sign;
        if (!(stream >> type >> key >> equal_sign)) {
            std::cerr << "\033[1;31mERROR: line " << l_count << " of file " << path << " contains an error of type or key\033[0m\n";
            exit(1);
        }
        if (equal_sign != "=") {
            std::cerr << "\033[1;31mERROR: expected \033[0m\033[31m=\033[1m, got \033[0m\033[31m" << equal_sign << "\033[1m.\nLine " << l_count << " of file " << path << "\033[0m\n";
            exit(1);
        }

        if (type == "int" || type == "float") {
            float value;
            if (stream >> value) float_vars[key] = value;
            else {
                std::cerr << "\033[1;31mERROR: " << type << " parameter " << key << " has no value.\nLine " << l_count << " of file " << path << "\033[0m\n";
                exit(1);
            }
        }
        else if (type == "bool") {
            std::string value;
            if (stream >> value) {
                if (value == "false" || value == "true") float_vars[key] = (value == "false" ? false : true);
                else {
                    std::cerr << "\033[1;31mERROR: bool parameter " << key << " should be \033[0m\033[31mtrue\033[1m of \033[0m\033[31mfalse\033[1m, got \033[0m\033[31m" << value << "\033[1m instead.\nLine " << l_count << " of file " << path << "\033[0m\n";
                    exit(1);
                }
            }
            else {
                std::cerr << "\033[1;31mERROR: bool parameter " << key << " has no value.\nLine " << l_count << " of file " << path << "\033[0m\n";
                exit(1);
            }
        }
        else if (type == "str") {
            std::string value;
            if (stream >> value) str_vars[key] = value;
            else {
                std::cerr << "\033[1;31mERROR: string parameter " << key << " has no value.\nLine " << l_count << " of file " << path << "\033[0m\n";
                exit(1);
            }
        }
    }
}

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
