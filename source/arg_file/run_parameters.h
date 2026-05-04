#pragma once
#include "parameters.h"

class RunParameters: public Parameters
{
protected:
    unsigned int seed;
    std::vector<Parameters> instances;
    std::vector<Parameters> algos;

    std::ostream& cout(std::ostream& c) const override;

    static bool is_opening_label(const std::string& keyword);
    static bool is_closing_label(const std::string& keyword);
    static bool is_valid_label(const std::string& label);
public:
    RunParameters(const std::string& path);
    
    void set_seed(unsigned int seed);
    void set_default_seed(unsigned int seed);
    unsigned int get_seed() const;

    const std::vector<Parameters>& get_algos() const;
    const std::vector<Parameters>& get_instances() const;
};