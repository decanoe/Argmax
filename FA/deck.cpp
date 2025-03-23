#include "deck.h"
#include <sstream>
#include <fstream>

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
                (_color == "none"       ? People::Score_type::None :
                (_color == "fix"        ? People::Score_type::Fix :
                (_color == "night"      ? People::Score_type::Night :
                (_color == "resource"   ? People::Score_type::Resource :
                (_color == "color1"     ? People::Score_type::Color1 :
                (_color == "color2"     ? People::Score_type::Color2 :
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

        std::string day;
        std::string _color;
        unsigned int map, plant, beast, rock;
        std::string _reward_type;
        while (stream >> day >> _color >> map >> plant >> beast >> rock >> _reward_type) {
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
                (_color == "none"       ? Sanctuary::Score_type::None :
                (_color == "fix"        ? Sanctuary::Score_type::Fix :
                (_color == "night"      ? Sanctuary::Score_type::Night :
                (_color == "resource"   ? Sanctuary::Score_type::Resource :
                (_color == "color1"     ? Sanctuary::Score_type::Color1 :
                (_color == "color2"     ? Sanctuary::Score_type::Color2 :
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

            std::shared_ptr<Sanctuary> sanctuary_card = std::make_shared<Sanctuary>(color, day=="night", map, plant, beast, rock);

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

CardPTR Deck::get_people(unsigned int index) const { return this->peoples[index]; }
CardPTR Deck::get_sanctuary(unsigned int index) const { return this->sanctuaries[index]; }
    
unsigned int Deck::get_people_count() const { return this->peoples.size(); }
unsigned int Deck::get_sanctuary_count() const { return this->sanctuaries.size(); }