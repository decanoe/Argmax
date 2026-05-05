#include "sat.h"
#include <fstream>
#include <string>

using namespace Problem;

Sat::Clause::Clause(int a, int b, int c):
    var_index_a(std::abs(a) - 1),
    var_index_b(std::abs(b) - 1),
    var_index_c(std::abs(c) - 1),
    state_a(a > 0),
    state_b(b > 0),
    state_c(c > 0) {}
bool Sat::Clause::evaluate(const std::vector<bool>& assignation) const {
    return assignation[var_index_a] == state_a && assignation[var_index_b] == state_b && assignation[var_index_c] == state_c;
}

Sat::Sat(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "\033[1;31mNo Qubo instance at file path \"" << path << "\"\n\033[0m";
        exit(1);
    }

    char p = 'c';
    std::string line;
    while (p != 'p')
    {
        std::getline(file, line);
        file >> p;
    }
    std::string cnf;
    file >> cnf;
    
    unsigned int nb_lines;
    file >> n >> nb_lines;
    
    clauses.reserve(nb_lines);

    int a, b, c, zero;
    for (size_t index = 0; index < nb_lines; index++)
    {
        file >> a >> b >> c >> zero;
        clauses.push_back(Clause(a, b, c));
    }
}

float Sat::evaluate(const std::vector<bool>& assignation) const {
    float score = 0;

    for (const Clause& clause : clauses)
        if (clause.evaluate(assignation)) score++;

    return score;
}

unsigned int Sat::get_nb_variables() const { return n; }