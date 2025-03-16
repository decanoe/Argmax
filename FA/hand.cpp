#include "hand.h"
#include <cmath>

// return true or false from proba (0 returns always false)
bool Hand::get_bool(float proba) {
    return ((float)std::rand() / (float)RAND_MAX) < proba;
}

Hand::Hand() {}
Hand::Hand(const Hand& h) {}

std::ostream& operator<<(std::ostream& c, const Hand& h) {
    return c << "(Hand)";
}





// instance specific
float Hand::score() const {
    return 0;
}
bool Hand::is_max_score(float score) const {
    return false;
}
int Hand::nb_args() const {
    return 0;
}
void Hand::mutate_arg(int index) {
    
}
void Hand::mutate_arg(int index, float probability) {
    if (get_bool(probability)) return;
}

std::unique_ptr<Instance> Hand::breed(const std::unique_ptr<Instance>& other_inst) {
    Hand* other = dynamic_cast<Hand*>(other_inst.get());
    if (other == nullptr) { std::cerr << "Instance is of wrong type, expected Hand !"; exit(-1); }
    
    return std::make_unique<Hand>(*this);
}
std::unique_ptr<Instance> Hand::clone() const {
    return std::make_unique<Hand>(*this);
}
std::unique_ptr<Instance> Hand::randomize_clone() const {
    return std::make_unique<Hand>(*this);
}
Hand& Hand::randomize() {
    return *this;
}