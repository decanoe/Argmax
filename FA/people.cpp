#include "people.h"

People::People(unsigned int index, Color color, bool night, unsigned int map_count, unsigned int plant_count, unsigned int beast_count, unsigned int rock_count):
    Card(color, night, map_count, plant_count, beast_count, rock_count), index(index), plant_cost(0), beast_cost(0), rock_cost(0), type(Score_type::None),
    col1(Card::Color::None), col2(Card::Color::None) {}

People& People::set_score_cost(unsigned int plant_cost, unsigned int beast_cost, unsigned int rock_cost) {
    this->plant_cost = plant_cost;
    this->beast_cost = beast_cost;
    this->rock_cost = rock_cost;
    return *this;
}
    
People& People::set_score_type(Score_type type, unsigned int score) {
    this->type = type;
    this->score_value = score;
    return *this;
}
People& People::set_score_type(Score_type type, unsigned int score, Card::ResourceType resource) {
    this->type = type;
    this->score_value = score;
    this->resource = resource;
    return *this;
}
People& People::set_score_type(Score_type type, unsigned int score, Card::Color color) {
    this->type = type;
    this->score_value = score;
    this->col1 = color;
    this->col2 = Card::Color::None;
    return *this;
}
People& People::set_score_type(Score_type type, unsigned int score, Card::Color color1, Card::Color color2) {
    this->type = type;
    this->score_value = score;
    this->col1 = color1;
    this->col2 = color2;
    return *this;
}

unsigned int People::get_index() const { return index; }

bool People::cost_paid(std::shared_ptr<Deck> deck, const std::vector<unsigned int>& sanctuaries, const std::vector<unsigned int>& cards, unsigned int card_index) const {
    unsigned int p = 0, b = 0, r = 0;
    
    for (size_t i = card_index; i < cards.size(); i++)
    {
        p += deck->get_people(cards[i])->get_plant_count();
        b += deck->get_people(cards[i])->get_beast_count();
        r += deck->get_people(cards[i])->get_rock_count();
    }
    for (unsigned int sanctuary : sanctuaries)
    {
        p += deck->get_sanctuary(sanctuary)->get_plant_count();
        b += deck->get_sanctuary(sanctuary)->get_beast_count();
        r += deck->get_sanctuary(sanctuary)->get_rock_count();
    }
    
    return plant_cost <= p && beast_cost <= b && rock_cost <= r;
}
unsigned int People::_color_score(std::shared_ptr<Deck> deck, const std::vector<unsigned int>& sanctuaries, const std::vector<unsigned int>& cards, unsigned int card_index) const {
    std::vector<unsigned int> col_counts = std::vector<unsigned int>(5, 0);
    for (size_t i = card_index; i < cards.size(); i++) col_counts[deck->get_people(cards[i])->get_color()] += 1;
    for (unsigned int sanctuary : sanctuaries) col_counts[deck->get_sanctuary(sanctuary)->get_color()] += 1;

    switch (this->type)
    {
    case Score_type::Color1: return col_counts[col1] * this->score_value;
    case Score_type::Color2: return (col_counts[col1] + col_counts[col2]) * this->score_value;
    case Score_type::Colorset: return std::min(std::min(col_counts[0], col_counts[1]), std::min(col_counts[2], col_counts[3])) * this->score_value;
    default: return 0;
    }
}
unsigned int People::score(std::shared_ptr<Deck> deck, const std::vector<unsigned int>& sanctuaries, const std::vector<unsigned int>& cards, unsigned int card_index) const {
    if (!cost_paid(deck, sanctuaries, cards, card_index)) return 0;

    unsigned int i = 0;
    switch (this->type)
    {
    case Score_type::None: return 0;
    case Score_type::Fix: return score_value;
    case Score_type::Night:
        for (size_t i = card_index; i < cards.size(); i++) i += deck->get_people(cards[i])->is_night();
        for (unsigned int sanctuary : sanctuaries) i += deck->get_sanctuary(sanctuary)->is_night();
        return i * score_value;
    case Score_type::Resource:
        switch (resource)
        {
        case ResourceType::Map:
            for (size_t i = card_index; i < cards.size(); i++) i += deck->get_people(cards[i])->get_map_count();
            for (unsigned int sanctuary : sanctuaries) i += deck->get_sanctuary(sanctuary)->get_map_count();
        case ResourceType::Plant:
            for (size_t i = card_index; i < cards.size(); i++) i += deck->get_people(cards[i])->get_plant_count();
            for (unsigned int sanctuary : sanctuaries) i += deck->get_sanctuary(sanctuary)->get_plant_count();
        case ResourceType::Beast:
            for (size_t i = card_index; i < cards.size(); i++) i += deck->get_people(cards[i])->get_beast_count();
            for (unsigned int sanctuary : sanctuaries) i += deck->get_sanctuary(sanctuary)->get_beast_count();
        case ResourceType::Rock:
            for (size_t i = card_index; i < cards.size(); i++) i += deck->get_people(cards[i])->get_rock_count();
            for (unsigned int sanctuary : sanctuaries) i += deck->get_sanctuary(sanctuary)->get_rock_count();
        }
        return i * score_value;
    default: return _color_score(deck, sanctuaries, cards, card_index);
    }
}