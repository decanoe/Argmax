#ifndef CLAUSE_HPP
#define CLAUSE_HPP
#include <vector>
#include <cstdint>

class Clause {
private:
    std::vector<std::uint64_t> normal_mask;
    std::vector<std::uint64_t> not_mask;
    unsigned int nb_var;
public:
    /// @brief Initialize the Clause with v1, v2 and v3 being indexes of the 3 boolean variables.
    ///        If the arguments are negative, this means there's a not in the formula
    /// @param v1 index of the 1st variable (negative if a negation is added)
    /// @param v2 index of the 2nd variable (negative if a negation is added)
    /// @param v3 index of the 3rd variable (negative if a negation is added)
    Clause(int v1, int v2, int v3);

    /// @brief returns true if the clause is fulfilled
    /// @param assignation the assignation of each variables
    bool evaluate(const std::vector<std::uint64_t>& assignation) const;
};

#endif