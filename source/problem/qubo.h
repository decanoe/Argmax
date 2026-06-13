#pragma once
#include <vector>
#include "problem.h"
#include "../utils/random_utils.h"

namespace Problem {
    class Qubo: public Problem
    {
    protected:
        unsigned int n;
        std::vector<unsigned int> indices1;
        std::vector<unsigned int> indices2;
        std::vector<float> _values;
    public:
        Qubo(const std::string& path);
        Qubo(unsigned int n, std::mt19937& rand);

        void save_to_file(const std::string& path) const;
        float evaluate(const std::vector<bool>& assignation) const override;

        unsigned int get_nb_variables() const override;
    };
}