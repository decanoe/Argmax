#pragma once
#include <vector>
#include <set>
#include <iostream>
#include "../source/random_utils.h"

class NK
{
private:
    unsigned int n, k;
    std::vector<std::vector<float>> weights;
    std::vector<std::set<unsigned int>> neighbors;
public:
    NK(const std::string& path);
    NK(unsigned int n, unsigned int k);

    void save_to_file(const std::string& path) const;
    float evaluate(const std::vector<bool>& assignation) const;

    unsigned int get_nb_variables() const;
};
