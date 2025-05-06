#pragma once
#include <vector>
#include <memory>
#include "deck.h"

class Card
{
public:
    enum Color { Red, Green, Yellow, Blue, None };
    enum ResourceType { Map, Plant, Beast, Rock };
    
    Card(unsigned int index, Color color, bool night, unsigned int map_count, unsigned int plant_count, unsigned int beast_count, unsigned int rock_count);

    bool is_night() const;
    Color get_color() const;
    unsigned int get_map_count() const;
    unsigned int get_plant_count() const;
    unsigned int get_beast_count() const;
    unsigned int get_rock_count() const;
    unsigned int get_index() const;

    struct HandInfo
    {
        unsigned int red = 0;
        unsigned int green = 0;
        unsigned int yellow = 0;
        unsigned int blue = 0;

        unsigned int map = 0;
        unsigned int plant = 0;
        unsigned int beast = 0;
        unsigned int rock = 0;

        unsigned int night = 0;

        HandInfo& add(const Card& card);
        unsigned int night_count() const;
        unsigned int color_count(Color color) const;
        unsigned int colorset() const;
        unsigned int resourceset() const;
        unsigned int resource_count(ResourceType resource) const;
    };
    
    virtual unsigned int score(const HandInfo&) const = 0;
    std::string color_code(Color c) const;
    std::string color_back_code(Color c) const;
    virtual std::vector<std::string> get_string_display() const = 0;
protected:
    unsigned int index;
    Color color;
    bool night = false;
    unsigned int map_count = 0, plant_count = 0, beast_count = 0, rock_count = 0;
};

using CardPTR = std::shared_ptr<Card>;