#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <cmath>

class ReversibleInstance
{
protected:
    float stored_score = NAN;
public:
    ReversibleInstance() = default;
    ReversibleInstance(const ReversibleInstance&) = default;

    virtual std::vector<float> to_normalized_point() const { return std::vector<float>(); }
    virtual float get_coord(unsigned int index) const = 0;
    virtual std::vector<std::string> to_debug_point() const { return std::vector<std::string>(); }
    virtual std::string get_arg_labels(unsigned int index) const { return "var"+std::to_string(index); }

    virtual float score_const() const = 0;
    virtual float score() {
        if (std::isnan(stored_score)) stored_score = score_const();
        return stored_score;
    }
    virtual bool is_max_score(float score) const { return false; }
    virtual unsigned int nb_args_max() const = 0;
    virtual unsigned int nb_args() const { return nb_args_max(); }
    virtual void mutate_arg(unsigned int index) = 0;
    virtual void mutate_arg(unsigned int index, float probability) = 0;
    virtual bool revert_last_mutation() = 0;

    virtual std::unique_ptr<ReversibleInstance> breed(const std::unique_ptr<ReversibleInstance>& other) = 0;
    virtual std::unique_ptr<ReversibleInstance> clone() const = 0;

    virtual std::unique_ptr<ReversibleInstance> randomize_clone() const = 0;

    virtual std::ostream& cout(std::ostream& c) const = 0;
};
std::ostream& operator<<(std::ostream&, std::unique_ptr<ReversibleInstance>&);
