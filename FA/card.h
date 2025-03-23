#pragma once
#include <vector>
#include <memory>
#include "deck.h"

class Card
{
public:
    enum Color { Red, Green, Blue, Yellow, None };
    enum ResourceType { Map, Plant, Beast, Rock };
    
    Card(Color color, bool night, unsigned int map_count, unsigned int plant_count, unsigned int beast_count, unsigned int rock_count);

    bool is_night() const;
    Color get_color() const;
    unsigned int get_map_count() const;
    unsigned int get_plant_count() const;
    unsigned int get_beast_count() const;
    unsigned int get_rock_count() const;
    virtual unsigned int get_index() const;
    
    virtual unsigned int score(std::shared_ptr<Deck> deck, const std::vector<unsigned int>& sanctuaries, const std::vector<unsigned int>& cards, unsigned int card_index) const = 0;
protected:
    Color color;
    bool night;
    unsigned int map_count, plant_count, beast_count, rock_count;
};

using CardPTR = std::shared_ptr<Card>;