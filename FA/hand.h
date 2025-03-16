#pragma once

#include "card.h"
#include <iostream>
#include "../source/instance.h"

class Hand: public Instance {
protected:
    std::vector<unsigned int> peoples;
    std::vector<unsigned int> sanctuaries;
    bool get_bool(float proba = .5);
public:
    Hand();
    Hand(const Hand&);
    
    // Instance specific
    float score() const override;
    bool is_max_score(float score) const override;
    int nb_args() const override;
    void mutate_arg(int index) override;
    void mutate_arg(int index, float probability) override;
    
    std::unique_ptr<Instance> breed(const std::unique_ptr<Instance>& other) override;
    std::unique_ptr<Instance> clone() const override;
    std::unique_ptr<Instance> randomize_clone() const;
    Hand& randomize();
    
    friend std::ostream& operator<<(std::ostream&, const Hand&);
};

std::ostream& operator<<(std::ostream&, const Hand&);