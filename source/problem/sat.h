#pragma once
#include <vector>
#include "problem.h"
#include "../utils/random_utils.h"

namespace Problem {
    class Sat: public Problem
    {
    protected:
        class Clause {
            public:
            Clause(int a, int b, int c);
            unsigned int var_index_a;
            unsigned int var_index_b;
            unsigned int var_index_c;
            bool state_a;
            bool state_b;
            bool state_c;

            bool evaluate(const std::vector<bool>& assignation) const;
        };

        unsigned int n;
        std::vector<Clause> clauses;
    public:
        Sat(const std::string& path);

        float evaluate(const std::vector<bool>& assignation) const override;

        unsigned int get_nb_variables() const override;
    };
}