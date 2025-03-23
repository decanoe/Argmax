#include "hand.h"
#include <cmath>
#include <algorithm>

// return true or false from proba (0 returns always false)
bool Hand::get_bool(float proba) {
    return ((float)std::rand() / (float)RAND_MAX) < proba;
}
unsigned int Hand::get_rand_index(unsigned int max) {
    return (unsigned int)std::rand() % max;
}
unsigned int Hand::get_rand_index(unsigned int max, std::vector<unsigned int> blacklist) {
    unsigned int i = get_rand_index(max);
    while (std::find(blacklist.begin(), blacklist.end(), i) != blacklist.end()) i = get_rand_index(max);
    return i;
}

Hand::Hand(std::shared_ptr<Deck> deck): deck(deck), peoples{0,1,2,3,4,5,6,7}, sanctuaries{0,1,2,3,4,5,6} {}
Hand::Hand(const Hand& h): deck(h.deck), peoples(h.peoples), sanctuaries(h.sanctuaries) {}

unsigned int Hand::nb_sanctuary() const {
    unsigned int count = 0;
    for (unsigned int i = 0; i < peoples.size() - 1; ++i)
        count += deck->get_people(peoples[i])->get_index() > deck->get_people(peoples[i+1])->get_index();
    return count;
}

std::ostream& Hand::cout(std::ostream& c) const { return c << *this; }
std::ostream& operator<<(std::ostream& c, const Hand& h) {
    c << "(Peoples : ";
    for (unsigned int people : h.peoples)
    {
        c << h.deck->get_people(people)->get_index() << ", ";
    }
    c << "\b\b\t | Sanct : ";
    unsigned int temp = h.nb_sanctuary();
    for (unsigned int s : h.sanctuaries)
    {
        if (temp == 0) break;
        c << s << ", ";
        temp--;
    }
    return c << "\b\b)";
}


// instance specific
float Hand::score() const {
    unsigned int s = 0;
    for (unsigned int i = 0; i < peoples.size(); ++i) s += deck->get_people(peoples[i])->score(deck, sanctuaries, peoples, i);
    for (unsigned int i = 0; i < nb_sanctuary(); ++i) s += deck->get_sanctuary(sanctuaries[i])->score(deck, sanctuaries, peoples, i);
    return s;
}
bool Hand::is_max_score(float score) const {
    return false;
}
int Hand::nb_args() const {
    return 9;
}
void Hand::mutate_arg(int index) {
    if (index == 8) {
        unsigned int replaced_index = get_rand_index(nb_sanctuary());
        std::vector<unsigned int> black_list = sanctuaries;
        for (unsigned int i = 0; i <= replaced_index; i++) black_list[i] = deck->get_sanctuary_count();
        
        sanctuaries[replaced_index] = get_rand_index(deck->get_sanctuary_count(), black_list);
        black_list = sanctuaries;
        for (unsigned int i = replaced_index; i <= 7; i++) if (sanctuaries[i] == sanctuaries[replaced_index]) {
            sanctuaries[i] = get_rand_index(deck->get_sanctuary_count(), black_list);
            black_list[i] = sanctuaries[i];
        }
    }
    else {
        peoples[index] = get_rand_index(deck->get_people_count(), peoples);
    }
}
void Hand::mutate_arg(int index, float probability) {
    if (get_bool(probability)) mutate_arg(index);
}

std::unique_ptr<Instance> Hand::breed(const std::unique_ptr<Instance>& other_inst) {
    Hand* other = dynamic_cast<Hand*>(other_inst.get());
    if (other == nullptr) { std::cerr << "Instance is of wrong type, expected Hand !"; exit(-1); }

    std::unique_ptr<Hand> h = std::make_unique<Hand>(this->deck);
    for (unsigned int i = 0; i < 8; ++i) {
        if (other->peoples[i] == peoples[i]) h->peoples[i] = peoples[i];
        else {
            bool already_in = false;
            for (unsigned int j = 0; j < i; ++j)
            {
                if (h->peoples[j] == peoples[i]) already_in = true;
            }
            if (already_in) h->peoples[i] = other->peoples[i];
            else h->peoples[i] = peoples[i];
        }
    }
    for (unsigned int i = 0; i < 7; ++i) {
        if (other->sanctuaries[i] == sanctuaries[i]) h->sanctuaries[i] = sanctuaries[i];
        else {
            bool already_in = false;
            for (unsigned int j = 0; j < i; ++j)
            {
                if (h->sanctuaries[j] == sanctuaries[i]) already_in = true;
            }
            if (already_in) h->sanctuaries[i] = other->sanctuaries[i];
            else h->sanctuaries[i] = sanctuaries[i];
        }
    }

    return h;
}
std::unique_ptr<Instance> Hand::clone() const {
    return std::make_unique<Hand>(*this);
}
std::unique_ptr<Instance> Hand::randomize_clone() const {
    return std::make_unique<Hand>(Hand(*this).randomize());
}
Hand& Hand::randomize() {
    peoples.clear();
    sanctuaries.clear();
    
    for (int i = 0; i < 8; i++) peoples.push_back(get_rand_index(deck->get_people_count(), peoples));
    for (int i = 0; i < 7; i++) sanctuaries.push_back(get_rand_index(deck->get_sanctuary_count(), sanctuaries));
    
    return *this;
}