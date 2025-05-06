#include "card.h"

Card::Card(unsigned int index, Color color, bool night, unsigned int map_count, unsigned int plant_count, unsigned int beast_count, unsigned int rock_count):
    index(index), color(color), night(night), map_count(map_count), plant_count(plant_count), beast_count(beast_count), rock_count(rock_count) {}

bool Card::is_night() const { return this->night; }
Card::Color Card::get_color() const { return this->color; }
unsigned int Card::get_map_count() const { return this->map_count; }
unsigned int Card::get_plant_count() const { return this->plant_count; }
unsigned int Card::get_beast_count() const { return this->beast_count; }
unsigned int Card::get_rock_count() const { return this->rock_count; }
unsigned int Card::get_index() const { return index; }

Card::HandInfo& Card::HandInfo::add(const Card& card) {
    switch (card.color)
    {
    case Color::Red:    this->red++;    break;
    case Color::Green:  this->green++;  break;
    case Color::Yellow: this->yellow++; break;
    case Color::Blue:   this->blue++;   break;
    default:   break;
    }
    this->map   += card.map_count;
    this->plant += card.plant_count;
    this->beast += card.beast_count;
    this->rock  += card.rock_count;
    
    this->night += card.night;
    return *this;
}
unsigned int Card::HandInfo::night_count() const { return this->night; }
unsigned int Card::HandInfo::color_count(Color color) const {
    switch (color)
    {
    case Color::Red:    return this->red;
    case Color::Green:  return this->green;
    case Color::Yellow: return this->yellow;
    case Color::Blue:   return this->blue;
    default:            return 0;
    }
}
unsigned int Card::HandInfo::colorset() const { return std::min(std::min(red, green), std::min(yellow, blue)); }
unsigned int Card::HandInfo::resourceset() const { return std::min(std::min(plant, beast), rock); }
unsigned int Card::HandInfo::resource_count(ResourceType resource) const {
    switch (resource)
    {
    case ResourceType::Map:     return this->map;
    case ResourceType::Plant:   return this->plant;
    case ResourceType::Beast:   return this->beast;
    case ResourceType::Rock:    return this->rock;
    default:                    return 0;
    }
}

std::string Card::color_code(Color c) const { return (c==Color::None)? "30;1" : std::to_string(31 + c); }
std::string Card::color_back_code(Color c) const { return (c==Color::None)? "40;1" : std::to_string(41 + c); }
