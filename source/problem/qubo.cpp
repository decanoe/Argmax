#include "qubo.h"
#include <fstream>
#include <string>

using namespace Problem;

Qubo::Qubo(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "\033[1;31mNo Qubo instance at file path \"" << path << "\"\n\033[0m";
        exit(1);
    }
    unsigned int nb_lines;
    file >> n >> nb_lines;
    
    indices1.reserve(nb_lines);
    indices2.reserve(nb_lines);
    _values.reserve(nb_lines);
    unsigned int i, j;
    float value;
    for (size_t index = 0; index < nb_lines; index++)
    {
        file >> i >> j >> value;
        indices1.push_back(i - 1);
        indices2.push_back(j - 1);
        _values.push_back(value);
    }
}
Qubo::Qubo(unsigned int n, std::mt19937& rand): n(n) {
    for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = i; j < n; j++)
    {
        int v = (int)RandomUtils::get_index(201, rand) - 100;
        indices1.push_back(i);
        indices2.push_back(j);
        _values.push_back(v);
    }
}

void Qubo::save_to_file(const std::string& path) const {
    std::ofstream file(path);
    file << n << " " << _values.size() << "\n";

    for (size_t k = 0; k < _values.size(); ++k) {
        file << (indices1[k] + 1) << " " << (indices2[k] + 1) << " " << _values[k] << "\n";
    }
}
float Qubo::evaluate(const std::vector<bool>& assignation) const {
    float score = 0.0f;
    const size_t m = _values.size();

    // copy once to avoid vector<bool> proxy cost
    std::vector<uint8_t> x(n);
    for (unsigned i = 0; i < n; ++i)
        x[i] = assignation[i];

    const uint32_t* i_idx = this->indices1.data();
    const uint32_t* j_idx = this->indices2.data();
    const float* v = this->_values.data();

    for (size_t k = 0; k < m; ++k) {
        score += v[k] * (x[i_idx[k]] & x[j_idx[k]]);
    }

    return score;
}

unsigned int Qubo::get_nb_variables() const { return n; }