#include "nk.h"
#include <fstream>
#include <string>

NK::NK(const std::string& path) {
    std::ifstream file(path);
    file >> n >> k;

    unsigned int index;
    neighbors.reserve(n);
    for (unsigned int i = 0; i < n; i++) {
        neighbors.push_back({});
        for (size_t j = 0; j < k+1; j++) {
            file >> index;
            neighbors[i].insert(index);
        }
    }

    float w;
    weights.reserve(n);
    for (unsigned int i = 0; i < n; i++) {
        weights.push_back(std::vector<float>());
        weights[i].reserve(2<<k);
        for (unsigned int j = 0; j < (2U<<k); j++) {
            file >> w;
            weights[i].push_back(w);
        }
    }
}
NK::NK(unsigned int n, unsigned int k): n(n), k(k) {
    weights.reserve(n);
    for (unsigned int i = 0; i < n; i++) {
        weights.push_back(std::vector<float>());
        weights[i].reserve(2<<k);
        for (unsigned int j = 0; j < (2U<<k); j++) {
            float v = RandomUtils::get_index(1000) + 1000 * RandomUtils::get_index(1000);
            weights[i].push_back(v / 1000000);
        }
    }

    neighbors.reserve(n);
    for (unsigned int i = 0; i < n; i++) {
        neighbors.push_back({i});
        for (size_t j = 0; j < k; j++) neighbors[i].insert(RandomUtils::get_index(n, neighbors[i]));
    }
}

void NK::save_to_file(const std::string& path) const {
    std::ofstream file(path);
    file << n << " " << k << "\n";

    for (const auto& i : neighbors) {
        for (unsigned int index : i) file << index << "\t";
        file << "\n";
    }

    for (const auto& i : weights) {
        for (float w : i) file << std::to_string(w) << " ";
        file << "\n";
    }
}
float NK::evaluate(const std::vector<bool>& assignation) const {
    float score = 0;

    for (unsigned int f = 0; f < n; f++)
    {
        unsigned int v = 0;
        for (unsigned int index : neighbors[f]) v = (v << 1) + assignation[index];
        score += weights[f][v];
    }

    return score;
}

unsigned int NK::get_nb_variables() const { return n; }