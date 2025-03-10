#ifndef CLAUSE_HPP
#define CLAUSE_HPP
#include "bit_string.h"

class Clause {
private:
    BitString normal_mask;
    BitString not_mask;
    unsigned int nb_var;
    enum State { Normal, Not, None };
    /// @brief set the state of a variable in the clause
    /// @param index index of the variable
    /// @param state state of the variable:
    /// none if the variable is not present
    /// normal if the variable is present
    /// not if the variable is present and preceded by a not
    void set_state(unsigned int index, State state);
public:
    /// @brief Initialize the Clause with v1, v2 and v3 being indexes of the 3 boolean variables.
    ///        If the arguments are negative, this means there's a not in the formula
    /// @param v1 index of the 1st variable (negative if a negation is added)
    /// @param v2 index of the 2nd variable (negative if a negation is added)
    /// @param v3 index of the 3rd variable (negative if a negation is added)
    /// @param print_size nb of bits to display when printing
    Clause(int v1, int v2, int v3, unsigned int print_size);

    /// @brief returns true if the clause is fulfilled
    /// @param assignation the assignation of each variables
    bool evaluate(const BitString& assignation) const;

    friend std::ostream& operator<<(std::ostream&, const Clause&);
};
std::ostream& operator<<(std::ostream&, const Clause&);

#endif