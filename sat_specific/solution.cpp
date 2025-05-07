#include "solution.h"
#include <cmath>

Solution::Solution(const Solution& s): assignation(s.assignation), valid_clauses(s.valid_clauses), formule(s.formule) { stored_score = s.stored_score; }
Solution::Solution(std::shared_ptr<Formule> f, std::vector<bool> assignation): assignation(assignation), formule(f) { reset_valid_clauses(); }
Solution::Solution(std::shared_ptr<Formule> f): assignation(f->get_nb_variables(), false), formule(f) { reset_valid_clauses(); }

void Solution::reset_valid_clauses() {
    valid_clauses = std::vector<bool>();
    valid_clauses.reserve(formule->get_nb_clauses());

    stored_score = 0;
    for (const auto& clause : formule->get_clauses()) {
        if (clause.evaluate(assignation)) {
            valid_clauses.push_back(true);
            stored_score++;
        }
        else
            valid_clauses.push_back(false);
    }
}

Solution& Solution::randomize() {
    for (size_t i = 0; i < assignation.size(); i++)
        assignation[i] = RandomUtils::get_bool(0.5f);
    
    reset_valid_clauses();
    return *this;
}
std::unique_ptr<Instance> Solution::randomize_clone() const {
    auto temp = std::make_unique<Solution>(formule, assignation);
    temp->randomize();
    return temp;
}

bool Solution::get(unsigned int index) const {
    if (index >= get_nb_variables()) {
        std::cerr << "index out of range: asking for variable " << index << " out of " << get_nb_variables() << "\n";
        exit(1);
    }
    return assignation[index];
}
void Solution::set(unsigned int index, bool value) {
    if (index >= get_nb_variables()) {
        std::cerr << "index out of range: setting value of variable " << index << " out of " << get_nb_variables() << "\n";
        exit(1);
    }
    if (assignation[index] == value) return;

    assignation[index] = value;

    for (unsigned int clause_index: formule->get_affected_clauses(index))
    {
        bool new_clause_state = formule->get_clause(clause_index).evaluate(assignation);
        stored_score += new_clause_state;
        stored_score -= valid_clauses[clause_index];

        valid_clauses[clause_index] = new_clause_state;
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
    for (bool v: s.assignation) if (v) c << "\033[1;34m1\033[0m"; else c << "0";
    return c;
}





// instance specific
float Solution::score_const() const {
    if (std::isnan(stored_score)) return formule->count_valid_clauses(assignation);
    return stored_score;
}
float Solution::score() {
    if (std::isnan(stored_score)) reset_valid_clauses();
    return stored_score;
}
bool Solution::is_max_score(float score) const {
    return formule->get_nb_clauses() == score;
}
unsigned int Solution::nb_args_max() const {
    return get_nb_variables();
}
void Solution::mutate_arg(unsigned int index) {
    set(index, !get(index));
    last_mutation = index;
}
void Solution::mutate_arg(unsigned int index, float probability) {
    if (RandomUtils::get_bool(probability)) mutate_arg(index);
}
bool Solution::revert_last_mutation() {
    if (last_mutation > formule->get_nb_variables()) return false;
    mutate_arg(last_mutation);
    last_mutation = -1;
    return true;
}

float Solution::get_coord(unsigned int index) const {
    return get(index);
}
std::vector<std::string> Solution::to_debug_point() const {
    std::vector<std::string> result = std::vector<std::string>(get_nb_variables(), "0");
    for (size_t i = 0; i < get_nb_variables(); i++) result[i] = get(i)?"1":"0";
    return result;
}
std::vector<float> Solution::to_normalized_point() const {
    std::vector<float> result = std::vector<float>(get_nb_variables(), 0);
    for (size_t i = 0; i < get_nb_variables(); i++) result[i] = get(i);
    return result;
}

std::unique_ptr<Instance> Solution::breed(const std::unique_ptr<Instance>& other_inst) {
    Solution* other = dynamic_cast<Solution*>(other_inst.get());
    if (other == nullptr) { std::cerr << "Instance is of wrong type, expected Solution !"; exit(-1); }

    Solution result = Solution(*this);
    for (size_t i = 0; i < assignation.size(); i++)
        if (other->assignation[i] == assignation[i])    result.assignation[i] = assignation[i];
        else                                            result.assignation[i] = RandomUtils::get_bool(0.5f);
    
    result.reset_valid_clauses();
    return std::unique_ptr<Solution>(&result);
}
std::unique_ptr<Instance> Solution::clone() const {
    return std::unique_ptr<Solution>(new Solution(*this));
}