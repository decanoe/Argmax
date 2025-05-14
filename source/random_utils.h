#pragma once
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <math.h>
#include <random>
#include <iostream>

namespace RandomUtils {
    bool get_bool(float proba);
    unsigned int get_index(unsigned int max);
    unsigned int get_index(unsigned int max, std::vector<unsigned int> blacklist);
    unsigned int get_index(unsigned int max, std::list<unsigned int> blacklist);
    unsigned int get_index(unsigned int max, std::set<unsigned int> blacklist);
    std::uint64_t get_uint64();

}