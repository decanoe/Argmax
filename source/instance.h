#pragma once
#include <memory>

class Instance
{
private:
public:
    Instance() = default;
    Instance(const Instance&) = default;

    virtual float score() const = 0;
    virtual int nb_args() const = 0;
    virtual void mutate_arg(int index) = 0;
    virtual void mutate_arg(int index, float probability) = 0;

    virtual std::unique_ptr<Instance> breed(std::unique_ptr<Instance> other) = 0;
    virtual std::unique_ptr<Instance> clone() const = 0;
};

