#include "solution.h"
#include <cmath>

// return true or false from proba (0 returns always false)
bool get_bool(float proba = .5) {
    return ((float)std::rand() / (float)RAND_MAX) < proba;
}

Solution::Solution(const Solution& s): Solution(s.formule, s.assignation) {}
Solution::Solution(std::shared_ptr<Formule> f, std::vector<std::uint64_t> assignation): formule(f), assignation(assignation) {}
Solution::Solution(std::shared_ptr<Formule> f): formule(f) {
    assignation = std::vector<std::uint64_t>(f->get_nb_variables() >> 6);

    for (int i = 0; i < assignation.size(); i++)
        assignation[i] = std::rand();
}

void Solution::randomize() {
    for (int i = 0; i < assignation.size(); i++)
        assignation[i] = std::rand();
}

bool Solution::get(unsigned int index) const {
    if (index >= get_nb_variables()) {
        std::cerr << "index out of range: asking for variable " << index << " out of " << get_nb_variables() << "\n";
        exit(1);
    }
    return (assignation[index >> 6] & (1 << (index % (1 << 6)))) != 0;
}

void Solution::set(unsigned int index, bool value) {
    if (index >= get_nb_variables()) {
        std::cerr << "index out of range: setting value of variable " << index << " out of " << get_nb_variables() << "\n";
        exit(1);
    }
    assignation[index >> 6] |= (1 << (index % (1 << 6)));
}

unsigned int Solution::get_nb_variables() const {
    return this->formule->get_nb_variables();
}

std::string Solution::to_string(bool compress) const {
    std::string result = "";
    for (int i = 0; i < get_nb_variables(); i++) {
        if (compress) result += std::to_string(get(i));
        else result += "x" + std::to_string(i + 1) + " = " + std::to_string(get(i)) + '\n';
    }
    return result;
}





// instance specific
float Solution::score() const {
    return formule->count_valid_clauses(assignation);
}
int Solution::nb_args() const {
    return get_nb_variables();
}
void Solution::mutate_arg(int index) {
    assignation[index >> 6] ^= 1 << (index % (1 << 6));
}
void Solution::mutate_arg(int index, float probability) {
    if (get_bool(probability)) mutate_arg(index);
}

std::unique_ptr<Instance> Solution::breed(std::unique_ptr<Instance> other) {
    return clone();
}
std::unique_ptr<Instance> Solution::clone() const {
    auto ptr = std::unique_ptr<Solution>{ new Solution(*this) }; // <- 1
    return ptr;
}