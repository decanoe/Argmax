#pragma once
#include <vector>

namespace Problem {
    class Problem
    {
    private:
    public:
        Problem() = default;
        Problem(const Problem&) = delete;

        virtual float evaluate(const std::vector<bool>& assignation) const = 0;

        virtual unsigned int get_nb_variables() const = 0;
    };
}