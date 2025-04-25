#include "deck.h"
#include <sstream>
#include <fstream>
#include <algorithm>

#include "people.h"
#include "sanctuary.h"

Deck::Deck(const std::string& peoples_path, const std::string& sanctuaries_path): peoples(), sanctuaries() {
    std::ifstream people_file = std::ifstream(peoples_path);
    if (!people_file.is_open()) {
        std::cerr << "\033[1;31mERROR: cannot open peoples file at \"" << peoples_path << "\" !\033[0m\n";
        exit(1);
    }
    std::ifstream sanctuary_file = std::ifstream(sanctuaries_path);
    if (!sanctuary_file.is_open()) {
        std::cerr << "\033[1;31mERROR: cannot open sanctuaries file at \"" << sanctuaries_path << "\" !\033[0m\n";
        exit(1);
    }

    // ================================== PEOPLE =======================================
    std::string line;
    while (std::getline(people_file, line))
    {
        if (line.size() == 0 || line[0] == '#') continue;
        
        std::istringstream stream(line);

        unsigned int index;
        std::string day;
        std::string _color;
        unsigned int map, plant, beast, rock;
        std::string _reward_type;
        while (stream >> index >> day >> _color >> map >> plant >> beast >> rock >> _reward_type) {
            /* #region get color */
            Card::Color color =
                (_color == "red"    ? Card::Color::Red :
                (_color == "blue"   ? Card::Color::Blue :
                (_color == "yellow" ? Card::Color::Yellow :
                (_color == "green"  ? Card::Color::Green :
                Card::Color::None ))));
            /* #endregion */
            /* #region get reward type */
            People::Score_type reward_type =
                (_reward_type == "none"       ? People::Score_type::None :
                (_reward_type == "fix"        ? People::Score_type::Fix :
                (_reward_type == "night"      ? People::Score_type::Night :
                (_reward_type == "resource"   ? People::Score_type::Resource :
                (_reward_type == "color1"     ? People::Score_type::Color1 :
                (_reward_type == "color2"     ? People::Score_type::Color2 :
                People::Score_type::Colorset))))));
            /* #endregion */
            
            /* #region get additional parameters */
            Card::ResourceType resource;
            Card::Color col1, col2;
            if (reward_type == People::Score_type::Resource) {
                std::string _resource;
                stream >> _resource;
                resource =
                    (_resource == "map"    ? Card::ResourceType::Map :
                    (_resource == "plant"   ? Card::ResourceType::Plant :
                    (_resource == "beast" ? Card::ResourceType::Beast :
                    Card::ResourceType::Rock)));
            }
            else if (reward_type == People::Score_type::Color1 || reward_type == People::Score_type::Color2) {
                std::string _col1;
                stream >> _col1;
                col1 =
                    (_col1 == "red"    ? Card::Color::Red :
                    (_col1 == "blue"   ? Card::Color::Blue :
                    (_col1 == "yellow" ? Card::Color::Yellow :
                    (_col1 == "green"  ? Card::Color::Green :
                    Card::Color::None ))));
                
                if (reward_type == People::Score_type::Color2) {
                    std::string _col2;
                    stream >> _col2;
                    col2 =
                        (_col2 == "red"    ? Card::Color::Red :
                        (_col2 == "blue"   ? Card::Color::Blue :
                        (_col2 == "yellow" ? Card::Color::Yellow :
                        (_col2 == "green"  ? Card::Color::Green :
                        Card::Color::None ))));
                }
            }
            /* #endregion */

            unsigned int score_value, plant_cost, beast_cost, rock_cost;
            if (reward_type != People::Score_type::None) stream >> score_value >> plant_cost >> beast_cost >> rock_cost;

            std::shared_ptr<People> people_card = std::make_shared<People>(index, color, day=="night", map, plant, beast, rock);

            if (reward_type == People::Score_type::None) this->peoples.push_back(people_card);
            else {
                people_card->set_score_cost(plant_cost, beast_cost, rock_cost);
                if      (reward_type == People::Score_type::Color1)   people_card->set_score_type(reward_type, score_value, col1);
                else if (reward_type == People::Score_type::Color2)   people_card->set_score_type(reward_type, score_value, col1, col2);
                else if (reward_type == People::Score_type::Resource) people_card->set_score_type(reward_type, score_value, resource);
                else people_card->set_score_type(reward_type, score_value);
                this->peoples.push_back(people_card);
            }
        }
    }

    // ================================== Sanctuary =======================================
    while (std::getline(sanctuary_file, line))
    {
        if (line.size() == 0 || line[0] == '#') continue;
        
        std::istringstream stream(line);

        unsigned int index;
        std::string day;
        std::string _color;
        unsigned int map, plant, beast, rock;
        std::string _reward_type;
        while (stream >> index >> day >> _color >> map >> plant >> beast >> rock >> _reward_type) {
            /* #region get color */
            Card::Color color =
                (_color == "red"    ? Card::Color::Red :
                (_color == "blue"   ? Card::Color::Blue :
                (_color == "yellow" ? Card::Color::Yellow :
                (_color == "green"  ? Card::Color::Green :
                Card::Color::None ))));
            /* #endregion */
            /* #region get reward type */
            Sanctuary::Score_type reward_type =
                (_reward_type == "none"       ? Sanctuary::Score_type::None :
                (_reward_type == "fix"        ? Sanctuary::Score_type::Fix :
                (_reward_type == "night"      ? Sanctuary::Score_type::Night :
                (_reward_type == "resource"   ? Sanctuary::Score_type::Resource :
                (_reward_type == "color1"     ? Sanctuary::Score_type::Color1 :
                (_reward_type == "color2"     ? Sanctuary::Score_type::Color2 :
                Sanctuary::Score_type::Colorset))))));
            /* #endregion */
            
            /* #region get additional parameters */
            Card::ResourceType resource;
            Card::Color col1, col2;
            if (reward_type == Sanctuary::Score_type::Resource) {
                std::string _resource;
                stream >> _resource;
                resource =
                    (_resource == "map"    ? Card::ResourceType::Map :
                    (_resource == "plant"   ? Card::ResourceType::Plant :
                    (_resource == "beast" ? Card::ResourceType::Beast :
                    Card::ResourceType::Rock)));
            }
            else if (reward_type == Sanctuary::Score_type::Color1 || reward_type == Sanctuary::Score_type::Color2) {
                std::string _col1;
                stream >> _col1;
                col1 =
                    (_col1 == "red"    ? Card::Color::Red :
                    (_col1 == "blue"   ? Card::Color::Blue :
                    (_col1 == "yellow" ? Card::Color::Yellow :
                    (_col1 == "green"  ? Card::Color::Green :
                    Card::Color::None ))));
                
                if (reward_type == Sanctuary::Score_type::Color2) {
                    std::string _col2;
                    stream >> _col2;
                    col2 =
                        (_col2 == "red"    ? Card::Color::Red :
                        (_col2 == "blue"   ? Card::Color::Blue :
                        (_col2 == "yellow" ? Card::Color::Yellow :
                        (_col2 == "green"  ? Card::Color::Green :
                        Card::Color::None ))));
                }
            }
            /* #endregion */

            unsigned int score_value;
            if (reward_type != Sanctuary::Score_type::None) stream >> score_value;

            std::shared_ptr<Sanctuary> sanctuary_card = std::make_shared<Sanctuary>(index, color, day=="night", map, plant, beast, rock);

            if (reward_type == Sanctuary::Score_type::None) this->sanctuaries.push_back(sanctuary_card);
            else {
                if      (reward_type == Sanctuary::Score_type::Color1)   sanctuary_card->set_score_type(reward_type, score_value, col1);
                else if (reward_type == Sanctuary::Score_type::Color2)   sanctuary_card->set_score_type(reward_type, score_value, col1, col2);
                else if (reward_type == Sanctuary::Score_type::Resource) sanctuary_card->set_score_type(reward_type, score_value, resource);
                else sanctuary_card->set_score_type(reward_type, score_value);
                this->sanctuaries.push_back(sanctuary_card);
            }
        }
    }

    people_file.close();
    sanctuary_file.close();
}
void Deck::keep_only_subset(const std::vector<unsigned int> indices) {
    auto it = peoples.begin();
    while (it != peoples.end()) {
        if (std::find(indices.begin(), indices.end(), (*it)->get_index()) == indices.end())
            it = peoples.erase(it);
        else
            it++;
    }
    
    it = sanctuaries.begin();
    while (it != sanctuaries.end()) {
        if (std::find(indices.begin(), indices.end(), (*it)->get_index()) == indices.end())
            it = sanctuaries.erase(it);
        else
            it++;
    }
}
std::vector<unsigned int> Deck::read_subset_file(const std::string& path) {
    std::ifstream file = std::ifstream(path);
    if (!file.is_open()) {
        std::cerr << "\033[1;31mERROR: cannot open subset file at \"" << path << "\" !\033[0m\n";
        exit(1);
    }
    
    std::vector<unsigned int> result = std::vector<unsigned int>();
    unsigned int index;
    while (!file.eof())
    {
        file >> index;
        result.push_back(index);
    }
    return result;
}

CardPTR Deck::get_people(unsigned int index) const { return this->peoples[index]; }
CardPTR Deck::get_sanctuary(unsigned int index) const { return this->sanctuaries[index]; }
    
unsigned int Deck::get_people_count() const { return this->peoples.size(); }
unsigned int Deck::get_sanctuary_count() const { return this->sanctuaries.size(); }

void Deck::display_deck(std::ostream& c) const {
    std::vector<std::vector<std::string>> people_str = std::vector<std::vector<std::string>>();
    std::vector<std::vector<std::string>> sanctuary_str = std::vector<std::vector<std::string>>();

    for (auto p : peoples) people_str.push_back(p->get_string_display());
    for (auto s : sanctuaries) sanctuary_str.push_back(s->get_string_display());

    c << "Peoples:\n";
    unsigned int card_height = people_str[0].size();
    unsigned int index = 0;
    unsigned int card_per_line = 10;
    while (index < people_str.size())
    {
        for (size_t line = 0; line < card_height; line++) {
            for (unsigned int i = 0; i < card_per_line; i++) {
                if (index + i >= people_str.size()) continue;
                c << people_str[index + i] [line] << " ";
            }

            c << "\n";
        }
        index+=card_per_line;
    }
    
    c << "Sanctuaries:\n";
    card_height = sanctuary_str[0].size();
    index = 0;
    card_per_line = 15;
    while (index < sanctuary_str.size())
    {
        for (size_t line = 0; line < card_height; line++) {
            for (unsigned int i = 0; i < card_per_line; i++) {
                if (index + i >= sanctuary_str.size()) continue;
                c << sanctuary_str[index + i] [line] << " ";
            }

            c << "\n";
        }
        index+=card_per_line;
    }
}
