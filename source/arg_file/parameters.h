#pragma once
#include <map>
#include <iostream>
#include <list>
#include <vector>

class Parameters
{
protected:
    std::string path;

    std::map<std::string, float> float_vars;
    std::map<std::string, std::string> str_vars;
    
    static bool is_blank(const std::string& string);
    void parse(std::istream& content, unsigned int offset_in_file);
    static void apply_replacements(const std::string& content, unsigned int offset_in_file, const std::string& path, std::vector<Parameters>& output, std::list<std::pair<std::string, std::list<std::string>>>::iterator replacements, std::list<std::pair<std::string, std::list<std::string>>>::iterator end);
    static void parse_with_iterators(std::istream& content, unsigned int offset_in_file, const std::string& path, std::vector<Parameters>& output);
    virtual std::ostream& cout(std::ostream& c) const;

    static void print_error(const std::string& line, unsigned int line_index, const std::string& path, const std::string& message);
    void print_error(const std::string& line, unsigned int line_index, const std::string& message) const;
public:
    Parameters(const std::string& path);
    Parameters(std::istream& content, unsigned int offset_in_file, const std::string& path);

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

    friend std::ostream& operator<<(std::ostream&, const Parameters&);
};
std::ostream& operator<<(std::ostream&, const Parameters&);