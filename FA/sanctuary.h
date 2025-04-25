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
    unsigned int score_value = 0;

    static std::string to_str_2(unsigned int value);
public:
    Sanctuary(unsigned int index, Color color, bool night, unsigned int map_count, unsigned int plant_count, unsigned int beast_count, unsigned int rock_count);
    
    Sanctuary& set_score_type(Score_type type, unsigned int score);
    Sanctuary& set_score_type(Score_type type, unsigned int score, Card::ResourceType resource);
    Sanctuary& set_score_type(Score_type type, unsigned int score, Card::Color color);
    Sanctuary& set_score_type(Score_type type, unsigned int score, Card::Color color1, Card::Color color2);

    unsigned int score(const HandInfo&) const override;

    std::vector<std::string> get_string_display() const override;
};