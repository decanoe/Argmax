#pragma once
#include "card.h"

class People: public Card
{
public:
    enum Score_type { None, Fix, Night, Resource, Color1, Color2, Colorset };
private:
    unsigned int index = 0;
    unsigned int plant_cost = 0, beast_cost = 0, rock_cost = 0;
    Score_type type;
    Card::Color col1, col2;
    Card::ResourceType resource;
    unsigned int score_value = 0;

    static std::string to_str_2(unsigned int value);
public:
    People(unsigned int index, Color color, bool night, unsigned int map_count, unsigned int plant_count, unsigned int beast_count, unsigned int rock_count);
    
    People& set_score_cost(unsigned int plant_cost, unsigned int beast_cost, unsigned int rock_cost);

    People& set_score_type(Score_type type, unsigned int score);
    People& set_score_type(Score_type type, unsigned int score, Card::ResourceType resource);
    People& set_score_type(Score_type type, unsigned int score, Card::Color color);
    People& set_score_type(Score_type type, unsigned int score, Card::Color color1, Card::Color color2);

    unsigned int get_index() const override;

    bool cost_paid(const HandInfo&) const;
    unsigned int score(const HandInfo&) const override;
    
    std::vector<std::string> get_string_display() const override;
};