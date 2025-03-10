#pragma once
#include "instance.h"
#include <iostream>
#include <functional>

namespace Argmax {
    struct simple_evolution_parameters
    {
        unsigned int generation_count = 1024;

        float mutation_probability = 0.1f;

        unsigned int population_size = 256;
    };
    struct evolution_parameters
    {
        unsigned int generation_count = 1024;

        float mutation_probability = 0.1f;

        unsigned int population_start_size = 256;
        unsigned int population_spawn_size = 64;
        unsigned int population_despawn_size = 64;
        unsigned int population_max_size = 512;

        enum DespawnCriteria { oldest, lowest_score };
        DespawnCriteria despawn_criteria = DespawnCriteria::lowest_score;
    };
    

    std::unique_ptr<Instance> hill_climb(const std::unique_ptr<Instance> start, unsigned int max_iter = 1024);
    std::unique_ptr<Instance> hill_climb_tab(const std::unique_ptr<Instance> start, size_t black_list_size = 3, unsigned int max_iter = 1024);
    std::unique_ptr<Instance> simple_evolution(std::function<std::unique_ptr<Instance>()> spawner, simple_evolution_parameters parameters);
}
