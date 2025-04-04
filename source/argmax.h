#pragma once
#include "file_data.h"
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

        unsigned int population_start_size = 256;
        unsigned int population_spawn_size = 64;
        unsigned int population_despawn_size = 64;
        
        
        unsigned int spawn_per_parent = 1;
        unsigned int competition_goup_size = 2;
        float mutation_probability = 0.1;

        enum ChildAlgo { none, hill_climb, tabu_search, lambda_mutation };
        ChildAlgo run_algo_on_child = ChildAlgo::none;
        unsigned int child_algo_budget = 128;
        unsigned int child_algo_parameter = 8;
        bool protect_child_from_despawn = true;
        
        
        float despawn_criteria_age_multiplier = 0;
        float despawn_criteria_diversity_multiplier = 0;
        
        unsigned int nb_islands = 1;
        int generation_between_migrations = -1;
        float percent_of_population_per_migrations = 0.05;
        
        bool debug_show_best = true;
        
        evolution_parameters(const FileData&);
    };
    std::ostream& operator<<(std::ostream&, const evolution_parameters&);

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
    std::unique_ptr<Instance> tabu_search(const std::unique_ptr<Instance> start, size_t black_list_size = 3, unsigned int max_iter = 1024);
    std::unique_ptr<Instance> simple_evolution(std::function<std::unique_ptr<Instance>()> spawner, simple_evolution_parameters parameters, bool show_best = false);
    std::unique_ptr<Instance> evolution(std::function<std::unique_ptr<Instance>()> spawner, evolution_parameters parameters);
}
