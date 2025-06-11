#pragma once
#include "file_data.h"
#include "reversible_instance.h"
#include "random_utils.h"
#include <iostream>
#include <fstream>
#include <functional>

namespace LocalSearch {
    unsigned int hill_climb_random          (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ofstream* out = nullptr, bool add_header = true);
    unsigned int hill_climb_first           (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ofstream* out = nullptr, bool add_header = true);
    unsigned int hill_climb_cycle           (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ofstream* out = nullptr, bool add_header = true);
    unsigned int hill_climb_best            (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ofstream* out = nullptr, bool add_header = true);
    unsigned int hill_climb_least           (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ofstream* out = nullptr, bool add_header = true);
    
    unsigned int hill_climb_greedy_first    (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ofstream* out = nullptr, bool add_header = true);
    unsigned int hill_climb_greedy_best     (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ofstream* out = nullptr, bool add_header = true);
    unsigned int hill_climb_greedy_least    (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ofstream* out = nullptr, bool add_header = true);
    
    unsigned int hill_climb_greedy_all_first(std::unique_ptr<Instance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ofstream* out = nullptr, bool add_header = true);
    unsigned int hill_climb_greedy_all_best (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ofstream* out = nullptr, bool add_header = true);
    unsigned int hill_climb_greedy_all_least(std::unique_ptr<Instance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0, std::ofstream* out = nullptr, bool add_header = true);
}