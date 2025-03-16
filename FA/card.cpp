#include "card.h"

Card::Card(Color color, bool night, unsigned int map_count, unsigned int plant_count, unsigned int beast_count, unsigned int rock_count):
    color(color), night(night), map_count(map_count), plant_count(plant_count), beast_count(beast_count), rock_count(rock_count) {}

bool Card::is_night() const { return this->night; }
Card::Color Card::get_color() const { return this->color; }
unsigned int Card::get_map_count() const { return this->map_count; }
unsigned int Card::get_plant_count() const { return this->plant_count; }
unsigned int Card::get_beast_count() const { return this->beast_count; }
unsigned int Card::get_rock_count() const { return this->rock_count; }