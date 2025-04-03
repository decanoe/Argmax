#include "solution copy.h"
#include <cmath>

// return true or false from proba (0 returns always false)
bool get_bool(float proba = .5) {
    return ((float)std::rand() / (float)RAND_MAX) < proba;
}

Solution::Solution(const Solution& s): Instance(s), assignation(s.assignation), valid_clauses(s.valid_clauses), formule(s.formule) {}
Solution::Solution(std::shared_ptr<Formule> f): assignation(f->get_nb_variables(), f->get_nb_variables()), valid_clauses(f->get_nb_clauses(), false), formule(f) { reset_valid_clauses(); }
Solution::Solution(std::shared_ptr<Formule> f, BitString assignation): assignation(assignation), valid_clauses(f->get_nb_clauses(), false), formule(f) { reset_valid_clauses(); }

void Solution::reset_valid_clauses() {
    stored_score = 0;
    for (unsigned int i = 0; i < formule->get_nb_clauses(); i++)
    {
        valid_clauses[i] = formule->get_clauses(i).evaluate(assignation);
        stored_score += valid_clauses[i];
    }
}

Solution& Solution::randomize() {
    assignation.randomize();
    reset_valid_clauses();
    return *this;
}
std::unique_ptr<Instance> Solution::randomize_clone() const {
    return std::make_unique<Solution>(formule, assignation.randomize_clone());
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
    if (assignation.get_bit(index) == value) return;
    assignation.set_bit(index, value);

    for (auto c : formule->get_affected_clauses(index)) {
        bool new_value = formule->get_clauses(c).evaluate(assignation);
        stored_score += new_value - valid_clauses[index];
        valid_clauses[index] = new_value;
    }
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

std::ostream& Solution::cout(std::ostream& c) const { return c << *this; }
std::ostream& operator<<(std::ostream& c, const Solution& s) {
    return c << s.assignation;
}





// instance specific
float Solution::score_const() const {
    if (std::isnan(stored_score)) {
        std::cout << "WARNING: solution with nan valued score !" << std::endl;
        return formule->count_valid_clauses(assignation);
    }
    return stored_score;
}
bool Solution::is_max_score(float score) const {
    return formule->get_nb_clauses() <= score;
}
int Solution::nb_args() const {
    return get_nb_variables();
}
void Solution::mutate_arg(int index) {
    set(index, !get(index));
}
void Solution::mutate_arg(int index, float probability) {
    if (get_bool(probability)) mutate_arg(index);
}

std::vector<float> Solution::to_point() const {
    std::vector<float> result = std::vector<float>(get_nb_variables(), 0);
    for (size_t i = 0; i < get_nb_variables(); i++) result[i] = get(i);
    return result;
}

std::unique_ptr<Instance> Solution::breed(const std::unique_ptr<Instance>& other_inst) {
    Solution* other = dynamic_cast<Solution*>(other_inst.get());
    if (other == nullptr) { std::cerr << "Instance is of wrong type, expected Solution !"; exit(-1); }

    BitString no_match_mask = this->assignation ^ other->assignation;
    BitString match_mask = ~no_match_mask;
    // std::cout << "\nthis:\t" << this->assignation << "\n";
    // std::cout << "other:\t" << other->assignation << "\n";
    // std::cout << "mask:\t" << no_match_mask << "\n";
    // exit(-1);
    
    BitString random = BitString(this->assignation);
    random.randomize();

    return std::make_unique<Solution>(this->formule,
        (this->assignation & match_mask) | (random & no_match_mask)
    );
}
std::unique_ptr<Instance> Solution::clone() const {
    return std::unique_ptr<Solution>(new Solution(*this));
}