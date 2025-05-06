#include "people.h"

People::People(unsigned int index, Color color, bool night, unsigned int map_count, unsigned int plant_count, unsigned int beast_count, unsigned int rock_count):
    Card(index, color, night, map_count, plant_count, beast_count, rock_count), plant_cost(0), beast_cost(0), rock_cost(0), type(Score_type::None),
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

bool People::cost_paid(const HandInfo& info) const {
    return
        plant_cost <= info.resource_count(ResourceType::Plant) &&
        beast_cost <= info.resource_count(ResourceType::Beast) &&
        rock_cost  <= info.resource_count(ResourceType::Rock);
}
unsigned int People::score(const HandInfo& info) const {
    if (!cost_paid(info)) return 0;
    
    switch (this->type)
    {
    case Score_type::None:      return 0;
    case Score_type::Fix:       return score_value;
    case Score_type::Night:     return info.night_count() * score_value;
    case Score_type::Resource:  return info.resource_count(resource) * score_value;
    case Score_type::ResourceSet:   return info.resourceset() * score_value;
    case Score_type::Color1:    return info.color_count(col1) * score_value;
    case Score_type::Color2:    return (info.color_count(col1) + info.color_count(col2)) * score_value;
    case Score_type::Colorset:  return info.colorset() * score_value;
    }
    return 0;
}

std::string People::to_str_2(unsigned int value) {
    if (value < 10) return " " + std::to_string(value);
    return std::to_string(value);
}
std::vector<std::string> People::get_string_display() const {
    std::string col = "\033[" + color_code(color) + "m";
    std::vector<std::string> result = std::vector<std::string>(7, col + "║\033[0m         " + col + "║\033[0m");
    result[0] = col + "╔═════════╗\033[0m";
    result[4] = col + "╟─────────╢\033[0m";
    result[6] = col + "╚═════════╝\033[0m";


    // header
    result[1] = col + "║\033[0m";
    result[1] += (is_night()?"\033[30m":"");
    result[1] += to_str_2(index);
    result[1] += (is_night()?"☾":"☀");
    result[1] += "\033[0m";
    result[1] += (map_count==0?"   ":" \033[33m⌂\033[0m ");
    for (unsigned int i = 0; i < 3 - std::min(3U, plant_count + beast_count + rock_count); i++) result[1] += " ";
    for (unsigned int i = 0; i < plant_count; i++) result[1] += "\033[32m♣\033[0m";
    for (unsigned int i = 0; i < beast_count; i++) result[1] += "\033[31m¥\033[0m";
    for (unsigned int i = 0; i < rock_count ; i++) result[1] += "\033[36m♦\033[0m";
    result[1] += col + "║\033[0m";
    
    // cost
    result[3] = col + "║\033[0m";
    for (unsigned int i = 0; i < 9 - std::min(9U, plant_cost + beast_cost + rock_cost); i++) result[3] += " ";
    for (unsigned int i = 0; i < plant_cost; i++) result[3] += "\033[32m♣\033[0m";
    for (unsigned int i = 0; i < beast_cost; i++) result[3] += "\033[31m¥\033[0m";
    for (unsigned int i = 0; i < rock_cost ; i++) result[3] += "\033[36m♦\033[0m";
    result[3] += col + "║\033[0m";

    // score
    result[5] = col + "║\033[0m";
    switch (type)
    {
    case Score_type::Fix:           result[5] += "      " + to_str_2(score_value); break;
    case Score_type::Night:         result[5] += "    " + to_str_2(score_value) + "/☾"; break;
    case Score_type::Resource: switch (resource) {
        case ResourceType::Plant:   result[5] += "    " + to_str_2(score_value) + "/\033[32m♣\033[0m"; break;
        case ResourceType::Beast:   result[5] += "    " + to_str_2(score_value) + "/\033[31m¥\033[0m"; break;
        case ResourceType::Rock:    result[5] += "    " + to_str_2(score_value) + "/\033[36m♦\033[0m"; break;
        case ResourceType::Map:     result[5] += "    " + to_str_2(score_value) + "/\033[33m⌂\033[0m"; break;
        } break;
    case Score_type::ResourceSet:   result[5] += "  " + to_str_2(score_value) + "/\033[32m♣\033[31m¥\033[36m♦\033[0m"; break;
    case Score_type::Color1:        result[5] += "   " + to_str_2(score_value) + "/\033[" + color_code(col1) + "m██\033[0m"; break;
    case Score_type::Color2:        result[5] += "   " + to_str_2(score_value) + "/\033[" + color_code(col1) + ";" + color_back_code(col2) + "m▀▀\033[0m"; break;
    case Score_type::Colorset:      result[5] += "   " + to_str_2(score_value) + "/\033[31;42m▀\033[33;44m▀\033[0m"; break;
    case Score_type::None:          result[5] += "        "; break;
    }
    result[5] += col + " ║\033[0m";

    return result;
}
