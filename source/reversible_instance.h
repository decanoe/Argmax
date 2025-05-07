#pragma once
#include "instance.h"

class ReversibleInstance: public Instance
{
protected:
public:
    ReversibleInstance() = default;
    ReversibleInstance(const ReversibleInstance&) = default;

    virtual bool revert_last_mutation() = 0;
};
