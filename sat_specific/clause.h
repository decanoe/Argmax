#ifndef CLAUSE_HPP
#define CLAUSE_HPP
#include <vector>
#include <iostream>

class Clause {
private:
public:
    unsigned int var1;
    unsigned int var2;
    unsigned int var3;

    bool state1;
    bool state2;
    bool state3;

    /// @brief Initialize the Clause with v1, v2 and v3 being indexes of the 3 boolean variables.
    ///        If the arguments are negative, this means there's a not in the formula
    /// @param v1 index of the 1st variable (negative if a negation is added)
    /// @param v2 index of the 2nd variable (negative if a negation is added)
    /// @param v3 index of the 3rd variable (negative if a negation is added)
    /// @param print_size nb of bits to display when printing
    Clause(int v1, int v2, int v3);

    /// @brief returns true if the clause is fulfilled
    /// @param assignation the assignation of each variables
    bool evaluate(const std::vector<bool>& assignation) const;

    friend std::ostream& operator<<(std::ostream&, const Clause&);
};
std::ostream& operator<<(std::ostream&, const Clause&);

#endif