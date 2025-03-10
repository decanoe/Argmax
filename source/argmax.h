#pragma once
#include "instance.h"
#include <iostream>

namespace Argmax {
    std::unique_ptr<Instance> hill_climb(const std::unique_ptr<Instance> start, unsigned int max_iter = 1024);
    std::unique_ptr<Instance> hill_climb_tab(const std::unique_ptr<Instance> start, size_t black_list_size = 3, unsigned int max_iter = 1024);
}
