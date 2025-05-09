#pragma once
#include <vector>
#include <memory>
#include <string>
#include <iostream>

class Card;
class Deck
{
private:
    std::vector<std::shared_ptr<Card>> peoples;
    std::vector<std::shared_ptr<Card>> sanctuaries;
public:
    Deck(const std::string& peoples_path, const std::string& sanctuaries_path);
    static std::vector<unsigned int> read_subset_file(const std::string& path);
    void keep_only_subset(const std::vector<unsigned int> indices);

    std::shared_ptr<Card> get_people(unsigned int index) const;
    std::shared_ptr<Card> get_sanctuary(unsigned int index) const;
    unsigned int get_people_index_by_index(unsigned int written_index) const;
    unsigned int get_sanctuary_index_by_index(unsigned int written_index) const;

    unsigned int get_people_count() const;
    unsigned int get_sanctuary_count() const;

    void display_deck(std::ostream& c) const;
};
