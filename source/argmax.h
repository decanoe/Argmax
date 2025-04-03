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

        unsigned int spawn_per_parent = 1;

        unsigned int competition_goup_size = 2;

        bool run_hc_on_child = true;

        enum DespawnCriteria { oldest, lowest_score, combined };
        DespawnCriteria despawn_criteria = DespawnCriteria::lowest_score;
        bool protect_child_from_despawn = true;
    };
    struct InstanceGenWrapper
    {
        std::unique_ptr<Instance> instance;
        unsigned int generation;

        InstanceGenWrapper(std::unique_ptr<Instance> i, unsigned int g)
        {
            instance = std::move(i);
            generation = g;
        }
    };

    float standard_derivation(std::vector<std::unique_ptr<Instance>>& population);
    float standard_derivation(std::vector<InstanceGenWrapper>& population);

    std::unique_ptr<Instance> hill_climb(const std::unique_ptr<Instance> start, unsigned int max_iter = 1024);
    std::unique_ptr<Instance> hill_climb_tab(const std::unique_ptr<Instance> start, size_t black_list_size = 3, unsigned int max_iter = 1024);
    std::unique_ptr<Instance> simple_evolution(std::function<std::unique_ptr<Instance>()> spawner, simple_evolution_parameters parameters, bool show_best = false);
    std::unique_ptr<Instance> evolution(std::function<std::unique_ptr<Instance>()> spawner, evolution_parameters parameters, bool show_best = false);
}
