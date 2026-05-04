#pragma once
#include <memory>
#include "../arg_file/parameters.h"
#include "problem.h"

namespace Problem {
    std::shared_ptr<Problem> create_problem(const Parameters& parameters, unsigned int seed, bool debug = true);
}