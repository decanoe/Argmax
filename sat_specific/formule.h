#ifndef FORMULE_HPP
#define FORMULE_HPP

#include "clause.h"
#include <iostream>
#include <fstream>
#include <string>

class Formule {
private:
    std::vector<Clause> clauses;
    unsigned int nb_variables;
public:
    /// @brief Creates a representation of a formule from a cnf (or txt) file in DIMACS CNF format
    /// @param path the cnf (or txt) file path
    Formule(std::string path);

    /// @brief returns the number of true clauses
    /// @param assignation the assignation of each variables
    unsigned int count_valid_clauses(const BitString& assignation) const;

    // @returns Number of clauses
    unsigned int get_nb_clauses() const;

    // @return Number of variables
    unsigned int get_nb_variables() const;
};

#endif