#include "solution.h"
#include <cmath>

// return true or false from proba (0 returns always false)
bool get_bool(float proba = .5) {
    return ((float)std::rand() / (float)RAND_MAX) < proba;
}

Solution::Solution(const Solution& s): Solution(s.formule, s.assignation) {}
Solution::Solution(std::shared_ptr<Formule> f, BitString assignation): assignation(assignation), formule(f) {}
Solution::Solution(std::shared_ptr<Formule> f): assignation(f->get_nb_variables(), f->get_nb_variables()), formule(f) {}

void Solution::randomize() {
    assignation.randomize();
}

bool Solution::get(unsigned int index) const {
    if (index >= get_nb_variables()) {
        std::cerr << "index out of range: asking for variable " << index << " out of " << get_nb_variables() << "\n";
        exit(1);
    }
    return assignation.get_bit(index);
}

void Solution::set(unsigned int index, bool value) {
    if (index >= get_nb_variables()) {
        std::cerr << "index out of range: setting value of variable " << index << " out of " << get_nb_variables() << "\n";
        exit(1);
    }
    assignation.set_bit(index, value);
}

unsigned int Solution::get_nb_variables() const {
    return this->formule->get_nb_variables();
}

std::string Solution::to_string(bool compress) const {
    std::string result = "";
    for (unsigned int i = 0; i < get_nb_variables(); i++) {
        if (compress) result += std::to_string(get(i));
        else result += "x" + std::to_string(i + 1) + " = " + std::to_string(get(i)) + '\n';
    }
    return result;
}

std::ostream& operator<<(std::ostream& c, const Solution& s) {
    return c << s.assignation;
}





// instance specific
float Solution::score() const {
    return formule->count_valid_clauses(assignation);
}
bool Solution::is_max_score(float score) const {
    return formule->get_nb_clauses() == score;
}
int Solution::nb_args() const {
    return get_nb_variables();
}
void Solution::mutate_arg(int index) {
    assignation.switch_bit(index);
}
void Solution::mutate_arg(int index, float probability) {
    if (get_bool(probability)) mutate_arg(index);
}

std::unique_ptr<Instance> Solution::breed(std::unique_ptr<Instance> other) {
    return clone();
}
std::unique_ptr<Instance> Solution::clone() const {
    return std::unique_ptr<Solution>(new Solution(*this));
}