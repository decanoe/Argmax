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
    
    weights.reserve(n);
    for (unsigned int line = 0; line < n; line++)
    {
        weights.push_back(std::vector<float>(line + 1, 0));
    }

    unsigned int i, j;
    float value;

    for (size_t index = 0; index < nb_lines; index++)
    {
        file >> i >> j >> value;
        set_weight(i - 1, j - 1, value);
    }
}
Qubo::Qubo(unsigned int n, std::mt19937& rand): n(n) {
    weights.reserve(n);
    for (unsigned int line = 0; line < n; line++)
    {
        weights.push_back(std::vector<float>(line + 1, 0));
    }

    for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = i; j < n; j++)
    {
        set_weight(i, j, RandomUtils::get_index(201, rand) - 100);
    }
}

float Qubo::get_weight(unsigned int i, unsigned int j) const {
    if (i <= j) return this->weights[j][i];
    else return this->weights[i][j];
}
void Qubo::set_weight(unsigned int i, unsigned int j, float weight) {
    if (i <= j) this->weights[j][i] = weight;
    else this->weights[i][j] = weight;
}

void Qubo::save_to_file(const std::string& path) const {
    unsigned int non_zero_weights = 0;
    for (std::vector<float> wl : weights) for (float w : wl) if (w != 0) non_zero_weights++;

    std::ofstream file(path);
    file << n << " " << non_zero_weights << "\n";

    for (unsigned int i = 0; i < n; i++)
    {
        for (unsigned int j = i; j < n; j++)
        {
            if (get_weight(i, j) != 0) file << (i + 1) << " " << (j + 1) << " " << get_weight(i, j) << "\n";
        }
    }
}
float Qubo::evaluate(const std::vector<bool>& assignation) const {
    float score = 0;

    for (unsigned int i = 0; i < n; i++)
    {
        if (!assignation[i]) continue;

        for (unsigned int j = i; j < n; j++)
        {
            if (assignation[j]) score += get_weight(i, j);
        }
    }

    return score;
}

unsigned int Qubo::get_nb_variables() const { return n; }