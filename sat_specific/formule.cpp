#include "formule.h"
#define __abs(a) (((a) >= 0) ? (a) : (-a))

Formule::Formule(const std::string& path) {
    std::ifstream file = std::ifstream(path);
    if (!file.is_open()) {
        std::cerr << "\033[1;31mERROR: cannot open file !\033[0m\n";
        exit(1);
    }
    
    std::string w;
    int nb_var = -1;
    int nb_clauses = -1;
    while (file >> w) {
        if (w != "p") continue;
        std::string cnf;
        file >> cnf;

        if (cnf != "cnf") {
            std::cerr << "\033[1;31mERROR: wrong format !\033[0m\n";
            exit(1);
        }

        file >> nb_var;
        file >> nb_clauses;

        std::getline(file, w); // skip end of line
        break;
    }
    if (nb_clauses == -1) {
        std::cerr << "\033[1;31mERROR: no initialisation line in file !\033[0m\n";
        exit(1);
    }

    clauses_containing_var = std::vector<std::list<unsigned int>>(nb_var, std::list<unsigned int>());
    clauses.reserve(nb_clauses);
    nb_variables = nb_var;

    int index = 0;
    int v1, v2, v3;
    while (file >> v1 >> v2 >> v3)
    {
        clauses.push_back(Clause(v1, v2, v3));

        if (v1 != 0) clauses_containing_var[clauses.back().var1].push_back(index);
        if (v2 != 0) clauses_containing_var[clauses.back().var2].push_back(index);
        if (v3 != 0) clauses_containing_var[clauses.back().var3].push_back(index);

        index++;
        std::getline(file, w); // skip end of line
    }
    if (index != nb_clauses) {
        std::cerr << "\033[1;31mERROR: not enough lines in file !\033[0m\n";
        exit(1);
    }

    file.close();
}

unsigned int Formule::count_valid_clauses(const std::vector<bool>& assignation) const {
    unsigned int count = 0;
    for (const Clause& clause : clauses)
        count += clause.evaluate(assignation);

    return count;
}

unsigned int Formule::get_nb_clauses() const {
    return clauses.size();
}
const Clause& Formule::get_clause(unsigned int index) const {
    if (index >= clauses.size()) {
        std::cerr << "index out of range: asking for clause " << index << " out of " << clauses.size() << "\n";
        exit(1);
    }
    return clauses[index];
}
const std::vector<Clause>& Formule::get_clauses() const { return clauses; }
const std::list<unsigned int>& Formule::get_affected_clauses(unsigned int variable_index) const {
    if (variable_index >= clauses_containing_var.size()) {
        std::cerr << "index out of range: asking for clause contining variable " << variable_index << " out of " << nb_variables << "\n";
        exit(1);
    }
    return clauses_containing_var[variable_index];
}

unsigned int Formule::get_nb_variables() const {
    return nb_variables;
}