#pragma once

#include "card.h"
#include <iostream>
#include "../source/instance.h"
#include "../source/random_utils.h"

class Hand: public Instance {
protected:
    std::shared_ptr<Deck> deck;
    std::vector<unsigned int> peoples;
    std::vector<unsigned int> sanctuaries;

    static std::string to_str_2(unsigned int value);
public:
    Hand(std::shared_ptr<Deck> deck);
    Hand(const Hand&);

    unsigned int nb_sanctuary() const;
    
    // Instance specific
    float score_const() const override;
    bool is_max_score(float score) const override;
    unsigned int nb_args() const override;
    void mutate_arg(unsigned int index) override;
    void mutate_arg(unsigned int index, float probability) override;
    
    std::unique_ptr<Instance> breed(const std::unique_ptr<Instance>& other) override;
    std::unique_ptr<Instance> clone() const override;
    std::unique_ptr<Instance> randomize_clone() const;
    Hand& randomize();

    std::vector<float> to_point() const override;
    std::vector<float> to_normalized_point() const override;
    std::string get_arg_labels(unsigned int index) const override;
    
    std::ostream& pretty_cout(std::ostream& c) const;
    std::ostream& cout(std::ostream& c) const override;
    friend std::ostream& operator<<(std::ostream&, const Hand&);
};

std::ostream& operator<<(std::ostream&, const Hand&);