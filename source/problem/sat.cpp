#include "sat.h"
#include <fstream>
#include <string>

using namespace Problem;

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
    
    clause_var_a.reserve(nb_lines);
    clause_var_b.reserve(nb_lines);
    clause_var_c.reserve(nb_lines);

    int a, b, c, zero;
    for (size_t index = 0; index < nb_lines; index++)
    {
        file >> a >> b >> c >> zero;
        clause_var_a.push_back(a);
        clause_var_b.push_back(b);
        clause_var_c.push_back(c);
    }
}

float Sat::evaluate(const std::vector<bool>& assignation) const {
    float score = 0;

    for (unsigned int i = 0; i < clause_var_a.size(); i++)
    {
        unsigned int a_index = std::abs(clause_var_a[i]) - 1;
        unsigned int b_index = std::abs(clause_var_b[i]) - 1;
        unsigned int c_index = std::abs(clause_var_c[i]) - 1;

        bool a_state = clause_var_a[i] > 0;
        bool b_state = clause_var_b[i] > 0;
        bool c_state = clause_var_c[i] > 0;

        if (assignation[a_index] != a_state) continue;
        if (assignation[b_index] != b_state) continue;
        if (assignation[c_index] != c_state) continue;

        score += 1;
    }

    return score;
}

unsigned int Sat::get_nb_variables() const { return n; }