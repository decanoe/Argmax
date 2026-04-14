#pragma once
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <math.h>
#include <random>
#include <iostream>

namespace RandomUtils {
    bool get_bool(float proba, std::mt19937 rand);
    unsigned int get_index(unsigned int max, std::mt19937 rand);
    unsigned int get_index(unsigned int max, std::vector<unsigned int> blacklist, std::mt19937 rand);
    unsigned int get_index(unsigned int max, std::list<unsigned int> blacklist, std::mt19937 rand);
    unsigned int get_index(unsigned int max, std::set<unsigned int> blacklist, std::mt19937 rand);
}