#include "hand.h"
#include <cmath>
#include <algorithm>

Hand::Hand(std::shared_ptr<Deck> deck): deck(deck), peoples{0,1,2,3,4,5,6,7}, sanctuaries{0,1,2,3,4,5,6} {}
Hand::Hand(std::shared_ptr<Deck> deck, std::vector<unsigned int> peoples, std::set<unsigned int> sanctuaries): deck(deck), peoples(peoples), sanctuaries(sanctuaries) {}
Hand::Hand(const Hand& h): deck(h.deck), peoples(h.peoples), sanctuaries(h.sanctuaries) {}

bool Hand::operator==(std::unique_ptr<Instance>& other) const {
    Hand* h = dynamic_cast<Hand*>(other.get());
    if (h == nullptr) return false;

    return h->peoples == peoples && h->sanctuaries == sanctuaries;
}

unsigned int Hand::compute_nb_sanctuary() const {
    unsigned int count = 0;
    for (unsigned int i = 0; i < peoples.size() - 1; ++i)
        count += deck->get_people(peoples[i])->get_index() < deck->get_people(peoples[i+1])->get_index();
    return count;
}
unsigned int Hand::nb_sanctuary() const { return sanctuaries.size(); }

std::string Hand::to_str_2(unsigned int value) {
    if (value < 10) return std::to_string(value) + " ";
    return std::to_string(value);
}
std::ostream& Hand::pretty_cout(std::ostream& c) const {
    std::vector<std::vector<std::string>> people_str = std::vector<std::vector<std::string>>();
    std::vector<std::vector<std::string>> sanctuary_str = std::vector<std::vector<std::string>>();
    std::vector<unsigned int> people_scores = std::vector<unsigned int>();
    std::vector<unsigned int> sanctuary_scores = std::vector<unsigned int>();

    Card::HandInfo info = Card::HandInfo();
    for (unsigned int s : sanctuaries) info.add(*deck->get_sanctuary(s));
    for (auto p = peoples.rbegin(); p != peoples.rend(); ++p)
    {
        people_scores.push_back(deck->get_people(*p)->score(info.add(*deck->get_people(*p))));
        people_str.push_back(deck->get_people(*p)->get_string_display());
    }
    for (unsigned int s : sanctuaries)
    {
        sanctuary_scores.push_back(deck->get_sanctuary(s)->score(info));
        sanctuary_str.push_back(deck->get_sanctuary(s)->get_string_display());
    }

    c << *this << "\n";
    for (auto card_score : sanctuary_scores) c << "   +" << to_str_2(card_score) << "    ";
    c << "\n";
    for (size_t line = 0; line < sanctuary_str[0].size(); line++) {
        for (auto card : sanctuary_str)
            c << card[line] << " ";
        c << "\n";
    }
    
    for (auto card_score = people_scores.rbegin(); card_score != people_scores.rend(); card_score++) c << "    +" << to_str_2(*card_score) << "     ";
    c << "\n";
    for (size_t line = 0; line < people_str[0].size(); line++) {
        for (auto card = people_str.rbegin(); card != people_str.rend(); card++)
            c << (*card)[line] << " ";
        c << "\n";
    }

    return c;
}
std::ostream& Hand::cout(std::ostream& c) const { return c << *this; }
std::ostream& operator<<(std::ostream& c, const Hand& h) {
    c << "(Peoples : ";
    for (unsigned int people : h.peoples)
    {
        c << h.deck->get_people(people)->get_index() << ", ";
    }
    c << "\b\b\t | Sanct : ";
    for (unsigned int s : h.sanctuaries)
    {
        c << h.deck->get_sanctuary(s)->get_index() << ", ";
    }
    return c << "\b\b)";
}

// instance specific
float Hand::score_const() const {
    if (!std::isnan(stored_score)) return stored_score;
    Card::HandInfo info = Card::HandInfo();
    unsigned int s = 0;
    for (unsigned int sanct : sanctuaries) info.add(*deck->get_sanctuary(sanct));
    for (auto p = peoples.rbegin(); p != peoples.rend(); ++p) s += deck->get_people(*p)->score(info.add(*deck->get_people(*p)));
    for (unsigned int sanct : sanctuaries) s += deck->get_sanctuary(sanct)->score(info);
    return s;
}
bool Hand::is_max_score(float score) const {
    return false;
}
unsigned int Hand::nb_args_max() const {
    return 8+7;
}
unsigned int Hand::nb_args() const {
    return 8+nb_sanctuary();
}
void Hand::mutate_arg(unsigned int index) {
    stored_score = NAN;
    if (index < 8) {
        unsigned int old = peoples[index];
        peoples[index] = RandomUtils::get_index(deck->get_people_count());
        for (unsigned int i = 0; i < 8; i++)
            if (i != index && peoples[i] == peoples[index]) {
                peoples[i] = old;
                break;
            }
        
        unsigned int nb = compute_nb_sanctuary();
        while (sanctuaries.size() < nb) sanctuaries.insert(RandomUtils::get_index(deck->get_sanctuary_count(), sanctuaries));
        while (sanctuaries.size() > nb) sanctuaries.erase(std::next(sanctuaries.begin(), RandomUtils::get_index(sanctuaries.size())));
    }
    else {
        index -= 8;
        sanctuaries.erase(std::next(sanctuaries.begin(), index));
        sanctuaries.insert(RandomUtils::get_index(deck->get_sanctuary_count(), sanctuaries));
    }
}
void Hand::mutate_arg(unsigned int index, float probability) {
    if (RandomUtils::get_bool(probability)) mutate_arg(index);
}

std::unique_ptr<Instance> Hand::breed(const std::unique_ptr<Instance>& other_inst) {
    Hand* other = dynamic_cast<Hand*>(other_inst.get());
    if (other == nullptr) { std::cerr << "Instance is of wrong type, expected Hand !"; exit(-1); }

    std::unique_ptr<Hand> h = std::make_unique<Hand>(this->deck, peoples, std::set<unsigned int>());
    bool valid = false;
    for (size_t i = 0; i < 1024 && !valid; i++)
    {
        valid = true;
        for (unsigned int i = 0; i < 8; i++) h->peoples[i] = RandomUtils::get_bool(.5f) ? peoples[i] : other->peoples[i];

        for (unsigned int i = 0; i < 8 && valid; i++)
        for (unsigned int j = 0; j < i && valid; j++) {
            if (h->peoples[i] == h->peoples[j]) valid = false;
        }
    }
    if (!valid) h->peoples = peoples;
    
    
    unsigned int nb = h->compute_nb_sanctuary();
    std::set<unsigned int> set1 = this->sanctuaries;
    std::set<unsigned int> set2 = other->sanctuaries;
    // fill with random sanctuaries from hands
    while (h->sanctuaries.size() < nb && (set1.size() != 0 || set2.size() != 0)) {
        if (set2.size() == 0 || (set1.size() != 0 && RandomUtils::get_bool(.5f))) {
            auto it = std::next(set1.begin(), RandomUtils::get_index(set1.size()));
            h->sanctuaries.insert(*it);
            set1.erase(it);
        }
        else {
            auto it = std::next(set2.begin(), RandomUtils::get_index(set2.size()));
            h->sanctuaries.insert(*it);
            set2.erase(it);
        }
    }
    // complete with random sanctuaries from deck
    while (h->sanctuaries.size() < nb) h->sanctuaries.insert(RandomUtils::get_index(deck->get_sanctuary_count(), sanctuaries));

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
    
    for (unsigned int i = 0; i < 8; i++) peoples.push_back(RandomUtils::get_index(deck->get_people_count(), peoples));
    unsigned int nb = compute_nb_sanctuary();
    for (unsigned int i = 0; i < nb; i++) sanctuaries.insert(RandomUtils::get_index(deck->get_sanctuary_count(), sanctuaries));
    
    stored_score = NAN;
    return *this;
}

float Hand::get_coord(unsigned int index) const {
    if (index < 8) return peoples[index];
    return *std::next(sanctuaries.begin(), index - 8);
}
std::vector<float> Hand::to_normalized_point() const {
    std::vector<float> result = std::vector<float>();
    result.reserve(8+nb_sanctuary());

    for (auto i : peoples)      result.push_back((float)deck->get_people(i)->get_index() / deck->get_people_count());
    for (auto i : sanctuaries)  result.push_back((float)deck->get_sanctuary(i)->get_index() / deck->get_sanctuary_count());

    return result;
}
std::vector<std::string> Hand::to_debug_point() const {
    std::vector<std::string> result = std::vector<std::string>();
    result.reserve(8+nb_sanctuary());

    for (auto i : peoples)      result.push_back(std::to_string(deck->get_people(i)->get_index()));
    for (auto i : sanctuaries)  result.push_back("s" + std::to_string(deck->get_sanctuary(i)->get_index()));

    return result;
}
std::string Hand::get_arg_labels(unsigned int index) const {
    if (index < 8) return "people" + std::to_string(index + 1);
    return "sanct" + std::to_string(index - 7);
}

