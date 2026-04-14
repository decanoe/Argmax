#pragma once
#include "../arg_file/file_data.h"
#include "../instance/reversible_instance.h"
#include "../utils/random_utils.h"
#include <iostream>
#include <fstream>
#include <functional>
#include <random>

namespace LocalSearch {
    enum class HC_Selection_Criterion { Random, First, Cycle, Best, Least };
    HC_Selection_Criterion get_HC_Selection_Criterion(const std::string& string);
    enum class GJ_Selection_Criterion { First, Best, Least };
    GJ_Selection_Criterion get_GJ_Selection_Criterion(const std::string& string);
    enum class GJ_Neighborhood_Scope { Full, Improve, Half };
    GJ_Neighborhood_Scope get_GJ_Neighborhood_Scope(const std::string& string);

    unsigned int hill_climb (std::unique_ptr<ReversibleInstance>& instance, HC_Selection_Criterion criterion, std::mt19937& rand, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ostream* out = nullptr, bool add_header = true);
    unsigned int greedy_jumper (std::unique_ptr<ReversibleInstance>& instance, GJ_Selection_Criterion criterion, GJ_Neighborhood_Scope scope, std::mt19937& rand, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ostream* out = nullptr, bool add_header = true);
}