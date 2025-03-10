#include "formule.h"

Formule::Formule(std::string path) {
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

    clauses.reserve(nb_clauses);
    nb_variables = nb_var;

    int index = 0;
    int v1, v2, v3;
    while (file >> v1 >> v2 >> v3)
    {
        clauses.push_back(Clause(v1, v2, v3, nb_variables));
        index++;
        std::getline(file, w); // skip end of line
    }
    if (index != nb_clauses) {
        std::cerr << "\033[1;31mERROR: not enough lines in file !\033[0m\n";
        exit(1);
    }

    file.close();
}

unsigned int Formule:: count_valid_clauses(const BitString& assignation) const {
    unsigned int count = 0;
    for (const Clause& clause : clauses)
        count += clause.evaluate(assignation);

    return count;
}

unsigned int Formule::get_nb_clauses() const {
    return clauses.size();
}

unsigned int Formule::get_nb_variables() const {
    return nb_variables;
}