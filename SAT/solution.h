#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <iostream>
#include <string>
#include "formule.h"
#include "../source/reversible_instance.h"
#include "../source/random_utils.h"

class Solution: public ReversibleInstance {
protected:
    std::vector<bool> assignation;
    std::vector<bool> valid_clauses;
    std::shared_ptr<Formule> formule;
    unsigned int last_mutation = -1;

    void reset_valid_clauses();
public:
    Solution(std::shared_ptr<Formule> f, std::vector<bool> assignation);
    Solution(std::shared_ptr<Formule> f);
    Solution(const Solution&);
    
    /// @brief randomizes the variables states in place and returns itself
    Solution& randomize();

    /// @param index The index of the value to return (generates an error if it's out of range)
    /// @return The value of the variable 
    bool get(unsigned int index) const;

    /// @param index Index of the variable to modify (generates an error if it's out of range)
    /// @param value The new value to set
    void set(unsigned int index, bool value);

    /// @return The number of variables in the solution
    unsigned int get_nb_variables() const;

    /// @param compress if true, the string returned is only 0 and 1
    /// @return A string that contains each variable and it's value
    std::string to_string(bool compress = false) const;


    // Instance specific
    float score_const() const override;
    float score() override;
    bool is_max_score(float score) const override;
    unsigned int nb_args_max() const override;
    void mutate_arg(unsigned int index) override;
    void mutate_arg(unsigned int index, float probability) override;
    bool revert_last_mutation() override;

    float get_coord(unsigned int index) const override;
    std::vector<float> to_normalized_point() const override;
    std::vector<std::string> to_debug_point() const override;

    std::unique_ptr<Instance> breed(const std::unique_ptr<Instance>& other) override;
    std::unique_ptr<Instance> clone() const override;
    std::unique_ptr<Instance> randomize_clone() const override;
    
    std::ostream& cout(std::ostream& c) const override;
    friend std::ostream& operator<<(std::ostream&, const Solution&);
};

std::ostream& operator<<(std::ostream&, const Solution&);

#endif