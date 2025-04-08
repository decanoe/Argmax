#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <cmath>

class Instance
{
protected:
    float stored_score = NAN;
public:
    Instance() = default;
    Instance(const Instance&) = default;

    virtual std::vector<float> to_normalized_point() const { return std::vector<float>(); }
    virtual std::vector<float> to_point() const { return std::vector<float>(); }
    virtual std::string get_arg_labels(unsigned int index) const { return "var"+std::to_string(index); }

    virtual float score_const() const = 0;
    virtual float score() {
        if (std::isnan(stored_score)) stored_score = score_const();
        return stored_score;
    }
    virtual bool is_max_score(float score) const { return false; }
    virtual unsigned int nb_args() const = 0;
    virtual void mutate_arg(unsigned int index) = 0;
    virtual void mutate_arg(unsigned int index, float probability) = 0;

    virtual std::unique_ptr<Instance> breed(const std::unique_ptr<Instance>& other) = 0;
    virtual std::unique_ptr<Instance> clone() const = 0;

    virtual std::ostream& cout(std::ostream& c) const = 0;
};

