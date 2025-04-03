#ifndef FORMULE_HPP
#define FORMULE_HPP

#include "clause.h"
#include <iostream>
#include <fstream>
#include <string>
#include <list>

class Formule {
private:
    std::vector<Clause> clauses;
    std::vector<std::list<unsigned int>> clauses_containing_var;
    unsigned int nb_variables;
public:
    /// @brief Creates a representation of a formule from a cnf (or txt) file in DIMACS CNF format
    /// @param path the cnf (or txt) file path
    Formule(const std::string& path);

    /// @brief returns the number of true clauses
    /// @param assignation the assignation of each variables
    unsigned int count_valid_clauses(const BitString& assignation) const;

    // @returns Number of clauses
    unsigned int get_nb_clauses() const;
    /// @brief returns the clause at this index
    /// @param index the index of the clause
    const Clause& get_clauses(unsigned int index) const;
    /// @brief returns a list of the clauses containing the variable of index <variable_index>
    /// @param variable_index the index of the variable
    const std::list<unsigned int>& get_affected_clauses(unsigned int variable_index) const;

    // @return Number of variables
    unsigned int get_nb_variables() const;
};

#endif