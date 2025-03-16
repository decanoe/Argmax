#include "sanctuary.h"

Sanctuary::Sanctuary(Color color, bool night, unsigned int map_count, unsigned int plant_count, unsigned int beast_count, unsigned int rock_count):
    Card(color, night, map_count, plant_count, beast_count, rock_count), type(Score_type::None), col1(Card::Color::None), col2(Card::Color::None) {}

Sanctuary& Sanctuary::set_score_type(Score_type type, unsigned int score) {
    this->type = type;
    this->score_value = score;
    return *this;
}
Sanctuary& Sanctuary::set_score_type(Score_type type, unsigned int score, Card::ResourceType resource) {
    this->type = type;
    this->score_value = score;
    this->resource = resource;
    return *this;
}
Sanctuary& Sanctuary::set_score_type(Score_type type, unsigned int score, Card::Color color) {
    this->type = type;
    this->score_value = score;
    this->col1 = color;
    this->col2 = Card::Color::None;
    return *this;
}
Sanctuary& Sanctuary::set_score_type(Score_type type, unsigned int score, Card::Color color1, Card::Color color2) {
    this->type = type;
    this->score_value = score;
    this->col1 = color1;
    this->col2 = color2;
    return *this;
}

unsigned int Sanctuary::_color_score(std::shared_ptr<Deck> deck, const std::vector<unsigned int>& sanctuaries, const std::vector<unsigned int>& cards, unsigned int card_index) const {
    std::vector<unsigned int> col_counts = std::vector<unsigned int>(5, 0);
    for (unsigned int card : cards) col_counts[deck->get_people(card)->get_color()] += 1;
    for (unsigned int sanctuary : sanctuaries) col_counts[deck->get_sanctuary(sanctuary)->get_color()] += 1;

    switch (this->type)
    {
    case Score_type::Color1: return col_counts[col1] * this->score_value;
    case Score_type::Color2: return (col_counts[col1] + col_counts[col2]) * this->score_value;
    case Score_type::Colorset: return std::min(std::min(col_counts[0], col_counts[1]), std::min(col_counts[2], col_counts[3])) * this->score_value;
    default: return 0;
    }
}
unsigned int Sanctuary::score(std::shared_ptr<Deck> deck, const std::vector<unsigned int>& sanctuaries, const std::vector<unsigned int>& cards, unsigned int card_index) const {
    unsigned int i = 0;
    switch (this->type)
    {
    case Score_type::None: return 0;
    case Score_type::Fix: return score_value;
    case Score_type::Night:
        for (unsigned int card : cards) i += deck->get_people(card)->is_night();
        for (unsigned int sanctuary : sanctuaries) i += deck->get_sanctuary(sanctuary)->is_night();
        return i * score_value;
    case Score_type::Resource:
        switch (resource)
        {
        case ResourceType::Map:
            for (unsigned int card : cards) i += deck->get_people(card)->get_map_count();
            for (unsigned int sanctuary : sanctuaries) i += deck->get_sanctuary(sanctuary)->get_map_count();
        case ResourceType::Plant:
            for (unsigned int card : cards) i += deck->get_people(card)->get_plant_count();
            for (unsigned int sanctuary : sanctuaries) i += deck->get_sanctuary(sanctuary)->get_plant_count();
        case ResourceType::Beast:
            for (unsigned int card : cards) i += deck->get_people(card)->get_beast_count();
            for (unsigned int sanctuary : sanctuaries) i += deck->get_sanctuary(sanctuary)->get_beast_count();
        case ResourceType::Rock:
            for (unsigned int card : cards) i += deck->get_people(card)->get_rock_count();
            for (unsigned int sanctuary : sanctuaries) i += deck->get_sanctuary(sanctuary)->get_rock_count();
        }
        return i * score_value;
    default: return _color_score(deck, sanctuaries, cards, card_index);
    }
}