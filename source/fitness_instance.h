#pragma once
#include <iostream>
#include <string>
#include <functional>
#include "./reversible_instance.h"
#include "./random_utils.h"

class FitnessInstance: public ReversibleInstance {
protected:
    std::vector<bool> assignation;
    unsigned int length;
    std::function<float(const std::vector<bool>&)> fitness_function;

    unsigned int last_mutation = -1;
    float last_score = NAN;
public:
    FitnessInstance(std::function<float(const std::vector<bool>&)> fitness_function, unsigned int length, std::vector<bool> assignation);
    FitnessInstance(std::function<float(const std::vector<bool>&)> fitness_function, unsigned int length);
    FitnessInstance(const FitnessInstance&);
    
    /// @brief randomizes the variables states in place and returns itself
    FitnessInstance& randomize();

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
    friend std::ostream& operator<<(std::ostream&, const FitnessInstance&);
};

std::ostream& operator<<(std::ostream&, const FitnessInstance&);