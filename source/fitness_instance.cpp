#include "fitness_instance.h"
#include <cmath>

FitnessInstance::FitnessInstance(const FitnessInstance& s): assignation(s.assignation), length(s.length), fitness_function(s.fitness_function) { stored_score = s.stored_score; }
FitnessInstance::FitnessInstance(std::function<float(const std::vector<bool>&)> fitness_function, unsigned int length, std::vector<bool> assignation): assignation(assignation), length(length), fitness_function(fitness_function) {}
FitnessInstance::FitnessInstance(std::function<float(const std::vector<bool>&)> fitness_function, unsigned int length): assignation(length, false), length(length), fitness_function(fitness_function) {}

FitnessInstance& FitnessInstance::randomize() {
    for (size_t i = 0; i < assignation.size(); i++)
        assignation[i] = RandomUtils::get_bool(0.5f);
    
    stored_score = NAN;
    return *this;
}
std::unique_ptr<Instance> FitnessInstance::randomize_clone() const {
    auto temp = std::make_unique<FitnessInstance>(fitness_function, length, assignation);
    temp->randomize();
    return temp;
}

bool FitnessInstance::get(unsigned int index) const {
    if (index >= get_nb_variables()) {
        std::cerr << "index out of range: asking for variable " << index << " out of " << get_nb_variables() << "\n";
        exit(1);
    }
    return assignation[index];
}
void FitnessInstance::set(unsigned int index, bool value) {
    if (index >= get_nb_variables()) {
        std::cerr << "index out of range: setting value of variable " << index << " out of " << get_nb_variables() << "\n";
        exit(1);
    }
    if (assignation[index] == value) return;

    assignation[index] = value;
    stored_score = NAN;
}

unsigned int FitnessInstance::get_nb_variables() const {
    return this->length;
}

std::string FitnessInstance::to_string(bool compress) const {
    std::string result = "";
    for (unsigned int i = 0; i < get_nb_variables(); i++) {
        if (compress) result += std::to_string(get(i));
        else result += "x" + std::to_string(i + 1) + " = " + std::to_string(get(i)) + '\n';
    }
    return result;
}

std::ostream& FitnessInstance::cout(std::ostream& c) const { return c << *this; }
std::ostream& operator<<(std::ostream& c, const FitnessInstance& s) {
    for (bool v: s.assignation) if (v) c << "\033[1;34m1\033[0m"; else c << "0";
    return c;
}





// instance specific
float FitnessInstance::score_const() const {
    if (std::isnan(stored_score)) return fitness_function(assignation);
    return stored_score;
}
unsigned int FitnessInstance::nb_args_max() const {
    return get_nb_variables();
}
void FitnessInstance::mutate_arg(unsigned int index) {
    last_score = stored_score;
    set(index, !get(index));
    last_mutation = index;
}
void FitnessInstance::mutate_arg(unsigned int index, float probability) {
    if (RandomUtils::get_bool(probability)) mutate_arg(index);
}
bool FitnessInstance::revert_last_mutation() {
    if (std::isnan(last_score)) return false;
    float temp = last_score;
    mutate_arg(last_mutation);
    stored_score = temp;
    return true;
}

float FitnessInstance::get_coord(unsigned int index) const {
    return get(index);
}
std::vector<std::string> FitnessInstance::to_debug_point() const {
    std::vector<std::string> result = std::vector<std::string>(get_nb_variables(), "0");
    for (size_t i = 0; i < get_nb_variables(); i++) result[i] = get(i)?"1":"0";
    return result;
}
std::vector<float> FitnessInstance::to_normalized_point() const {
    std::vector<float> result = std::vector<float>(get_nb_variables(), 0);
    for (size_t i = 0; i < get_nb_variables(); i++) result[i] = get(i);
    return result;
}

std::unique_ptr<Instance> FitnessInstance::breed(const std::unique_ptr<Instance>& other_inst) {
    FitnessInstance* other = dynamic_cast<FitnessInstance*>(other_inst.get());
    if (other == nullptr) { std::cerr << "Instance is of wrong type, expected FitnessInstance !"; exit(-1); }

    FitnessInstance* result = new FitnessInstance(*this);
    for (size_t i = 0; i < assignation.size(); i++)
        if (other->assignation[i] == assignation[i])    result->assignation[i] = assignation[i];
        else                                            result->assignation[i] = RandomUtils::get_bool(0.5f);
    
    result->stored_score = NAN;
    return std::unique_ptr<FitnessInstance>(result);
}
std::unique_ptr<Instance> FitnessInstance::clone() const {
    return std::unique_ptr<FitnessInstance>(new FitnessInstance(*this));
}