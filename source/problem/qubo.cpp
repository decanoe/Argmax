#include "qubo.h"
#include <fstream>
#include <string>

using namespace Problem;

Qubo::Weight::Weight(unsigned int i, unsigned int j, float v): index1(i), index2(j), value(v) {}

Qubo::Qubo(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "\033[1;31mNo Qubo instance at file path \"" << path << "\"\n\033[0m";
        exit(1);
    }
    unsigned int nb_lines;
    file >> n >> nb_lines;
    
    weights.reserve(nb_lines);
    unsigned int i, j;
    float value;
    for (size_t index = 0; index < nb_lines; index++)
    {
        file >> i >> j >> value;
        weights.push_back(Weight(i-1, j-1, value));
    }
}
Qubo::Qubo(unsigned int n, std::mt19937& rand): n(n) {
    for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = i; j < n; j++)
    {
        int v = (int)RandomUtils::get_index(201, rand) - 100;
        if (v != 0) weights.push_back(Weight(i, j, v));
    }
}

void Qubo::save_to_file(const std::string& path) const {
    std::ofstream file(path);
    file << n << " " << weights.size() << "\n";

    for (const Weight& weight : weights) file << (weight.index1 + 1) << " " << (weight.index2 + 1) << " " << weight.value << "\n";
}
float Qubo::evaluate(const std::vector<bool>& assignation) const {
    float score = 0;

    for (const Weight& weight : weights) if (assignation[weight.index1] && assignation[weight.index2]) score += weight.value;

    return score;
}

unsigned int Qubo::get_nb_variables() const { return n; }