#pragma once
#include "card.h"

class Sanctuary: public Card
{
public:
    enum Score_type { None, Fix, Night, Resource, Color1, Color2, Colorset };
private:
    Score_type type;
    Card::Color col1, col2;
    Card::ResourceType resource;
    unsigned int score_value;

    unsigned int _color_score(std::shared_ptr<Deck> deck, const std::vector<unsigned int>& sanctuaries, const std::vector<unsigned int>& cards, unsigned int card_index) const;
public:
    Sanctuary(Color color, bool night, unsigned int map_count, unsigned int plant_count, unsigned int beast_count, unsigned int rock_count);
    
    Sanctuary& set_score_type(Score_type type, unsigned int score);
    Sanctuary& set_score_type(Score_type type, unsigned int score, Card::ResourceType resource);
    Sanctuary& set_score_type(Score_type type, unsigned int score, Card::Color color);
    Sanctuary& set_score_type(Score_type type, unsigned int score, Card::Color color1, Card::Color color2);

    unsigned int score(std::shared_ptr<Deck> deck, const std::vector<unsigned int>& sanctuaries, const std::vector<unsigned int>& cards, unsigned int card_index) const override;
};