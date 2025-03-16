#pragma once
#include "card.h"

class People: public Card
{
public:
    enum Score_type { None, Fix, Night, Resource, Color1, Color2, Colorset };
private:
    unsigned int index;
    unsigned int plant_cost, beast_cost, rock_cost;
    Score_type type;
    Card::Color col1, col2;
    Card::ResourceType resource;
    unsigned int score_value;

    unsigned int _color_score(std::shared_ptr<Deck> deck, const std::vector<unsigned int>& sanctuaries, const std::vector<unsigned int>& cards, unsigned int card_index) const;
public:
    People(unsigned int index, Color color, bool night, unsigned int map_count, unsigned int plant_count, unsigned int beast_count, unsigned int rock_count);
    
    People& set_score_cost(unsigned int plant_cost, unsigned int beast_cost, unsigned int rock_cost);

    People& set_score_type(Score_type type, unsigned int score);
    People& set_score_type(Score_type type, unsigned int score, Card::ResourceType resource);
    People& set_score_type(Score_type type, unsigned int score, Card::Color color);
    People& set_score_type(Score_type type, unsigned int score, Card::Color color1, Card::Color color2);

    bool cost_paid(std::shared_ptr<Deck> deck, const std::vector<unsigned int>& sanctuaries, const std::vector<unsigned int>& cards, unsigned int card_index) const;
    unsigned int score(std::shared_ptr<Deck> deck, const std::vector<unsigned int>& sanctuaries, const std::vector<unsigned int>& cards, unsigned int card_index) const override;
};