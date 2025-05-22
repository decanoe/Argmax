#pragma once
#include "file_data.h"
#include "reversible_instance.h"
#include "random_utils.h"
#include <iostream>
#include <fstream>
#include <functional>

namespace LocalSearch {
    void hill_climb_one             (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, std::ofstream* out = nullptr);
    void hill_climb_first           (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, std::ofstream* out = nullptr);
    void hill_climb_best            (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, std::ofstream* out = nullptr);
    void hill_climb_greedy_first    (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, std::ofstream* out = nullptr);
    void hill_climb_greedy_best     (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, std::ofstream* out = nullptr);
    void hill_climb_greedy_all_first(std::unique_ptr<Instance>& instance, unsigned int budget = 1024, std::ofstream* out = nullptr);
    void hill_climb_greedy_all_best (std::unique_ptr<Instance>& instance, unsigned int budget = 1024, std::ofstream* out = nullptr);
}