#pragma once
#include <map>
#include <string>
#include <list>
#include <iostream>

class FileData
{
private:
    std::string path;
    unsigned int seed;

    std::map<std::string, float> float_vars;
    std::map<std::string, std::string> str_vars;
public:
    FileData(std::istream& content, const std::string& path);
    
    void set_seed(unsigned int seed);
    void set_default_seed(unsigned int seed);
    unsigned int get_seed() const ;

    bool get_bool(const std::string&) const;
    int get_int(const std::string&) const;
    unsigned int get_uint(const std::string&) const;
    float get_float(const std::string&) const;
    const std::string& get_string(const std::string&) const;

    bool get_bool(const std::string&, bool default_value) const;
    int get_int(const std::string&, int default_value) const;
    unsigned int get_uint(const std::string&, int default_value) const;
    float get_float(const std::string&, float default_value) const;
    const std::string& get_string(const std::string&, const std::string& default_value) const;

    bool contains_bool(const std::string&) const;
    bool contains_int(const std::string&) const;
    bool contains_uint(const std::string&) const;
    bool contains_float(const std::string&) const;
    bool contains_string(const std::string&) const;

    friend std::ostream& operator<<(std::ostream&, const FileData&);
};
std::ostream& operator<<(std::ostream&, const FileData&);

std::list<FileData> generate_file_data(const std::string& path, bool allow_import = true);