#pragma once
#include <vector>
#include "problem.h"
#include "../utils/random_utils.h"

namespace Problem {
    class Sat: public Problem
    {
    protected:
        unsigned int n;
        std::vector<int> clause_var_a;
        std::vector<int> clause_var_b;
        std::vector<int> clause_var_c;
    public:
        Sat(const std::string& path);

        float evaluate(const std::vector<bool>& assignation) const override;

        unsigned int get_nb_variables() const override;
    };
}