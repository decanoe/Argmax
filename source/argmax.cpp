#include "argmax.h"
#include <chrono>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <random>

float get_time_from(std::chrono::system_clock::time_point point)
{
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_time = end - point;
    double elapsed_seconds = elapsed_time.count();
    return elapsed_seconds;

    // auto start = std::chrono::system_clock::now();
    // float elapsed += get_time_from(start);
}

/// @brief changes <instance> to its best scoring neighbor
/// @param instance
/// @return true if a better neighbor was found
bool change_to_better_neighbor(std::unique_ptr<Instance> &instance)
{
    std::unique_ptr<Instance> best = nullptr;
    float score = instance->score();

    for (int i = 0; i < instance->nb_args(); i++)
    {
        std::unique_ptr<Instance> mutation = instance->clone();
        mutation->mutate_arg(i);
        float mutation_score = mutation->score();

        if (mutation_score > score)
        {
            best = std::move(mutation);
            score = mutation_score;
        }
    }

    if (best == nullptr)
        return false;
    instance = std::move(best);
    return true;
}
std::unique_ptr<Instance> Argmax::hill_climb(const std::unique_ptr<Instance> start, unsigned int max_iter)
{
    std::unique_ptr<Instance> result = start->clone();
    unsigned int i = 0;
    while (change_to_better_neighbor(result) && ++i < max_iter)
        ;
    return result;
}

/// @brief changes <instance> to its best scoring neighbor
/// @param instance
/// @return index of the argument changed (-1 if max was reach)
int change_to_best_neighbor(std::unique_ptr<Instance> &instance, const std::list<int> &black_list)
{
    std::unique_ptr<Instance> best = nullptr;
    float score = instance->score();
    int index = -1;

    for (int i = 0; i < instance->nb_args(); i++)
    {
        if (std::find(black_list.begin(), black_list.end(), i) != black_list.end())
            continue;

        std::unique_ptr<Instance> mutation = instance->clone();
        mutation->mutate_arg(i);
        float mutation_score = mutation->score();

        if (mutation_score > score || index == -1)
        {
            index = i;
            best = std::move(mutation);
            score = mutation_score;

            if (best->is_max_score(score))
            {
                instance = std::move(best);
                return -1;
            }
        }
    }

    instance = std::move(best);
    return index;
}
std::unique_ptr<Instance> Argmax::hill_climb_tab(const std::unique_ptr<Instance> start, size_t black_list_size, unsigned int max_iter)
{
    std::list<int> black_list;
    std::unique_ptr<Instance> result = start->clone();
    std::unique_ptr<Instance> best = result->clone();
    for (size_t i = 0; i < max_iter; i++)
    {
        int index = change_to_best_neighbor(result, black_list);
        if (index == -1)
            return result;

        if (result->score() > best->score())
            best = result->clone();

        black_list.push_front(index);
        if (black_list.size() > black_list_size)
            black_list.pop_back();
    }

    return best;
}

std::unique_ptr<Instance> Argmax::simple_evolution(std::function<std::unique_ptr<Instance>()> spawner, simple_evolution_parameters parameters, bool show_best)
{
    std::vector<std::unique_ptr<Instance>> population = std::vector<std::unique_ptr<Instance>>();
    population.reserve(parameters.population_size);
    for (unsigned int i = 0; i < parameters.population_size; i++)
        population.push_back(spawner());

    std::cout << "\n";

    std::unique_ptr<Instance> best = nullptr;
    float best_score = 0;
    int progress_percent = 0;
    for (unsigned int g = 0; g < parameters.generation_count; g++)
    {
        std::unique_ptr<Instance> parent_1 = nullptr;
        float score_1 = 0;
        std::unique_ptr<Instance> parent_2 = nullptr;
        float score_2 = 0;

        // get parents
        for (std::unique_ptr<Instance> &instance : population)
        {
            float score = instance->score();

            if (score > best_score || best == nullptr)
            {
                best_score = score;
                best = instance->clone();
                if (best->is_max_score(best_score))
                {
                    std::cout << "\r                                                                                         \r";
                    return best;
                }
            }

            if (score > score_1 || parent_1 == nullptr)
            {
                score_1 = score;
                parent_1 = std::move(instance);
            }
            else if (score > score_2 || parent_2 == nullptr)
            {
                score_2 = score;
                parent_2 = std::move(instance);
            }
        }

        if (parent_1 == nullptr || parent_2 == nullptr)
        {
            std::cerr << "\033[1;31mNot enough instances in the population, can't find parents !\033[0m";
            exit(-1);
        }

        // make new population
        for (unsigned int i = 0; i < parameters.population_size; i++)
        {
            population[i] = parent_1->breed(parent_2);

            // mutate offspring
            for (int arg = 0; arg < population[i]->nb_args(); arg++)
                population[i]->mutate_arg(arg, parameters.mutation_probability);
        }

        // pretty print to wait
        int p = 100 * (float)g / parameters.generation_count;
        if (p != progress_percent)
        {
            progress_percent = p;
            std::string t = "\033[A\r\033[Kprogress: " + std::to_string(progress_percent);
            if (p < 10)
                t += " ";
            t += "% [";
            for (int i = 0; i < p / 10; i++)
                t += "="; //"▆";
            for (int i = p / 10; i < 10; i++)
                t += " ";

            std::cout << t + "] best in generation: " + std::to_string(score_1) + "\t overall best: " + std::to_string(best_score) + "\n\033[Kbest instance of gen: ";
            if (show_best)
                parent_1->cout(std::cout);
        }
    }

    std::cout << "\r\033[K\033[A\r\033[K\r";
    std::cout << "population:\n";
    for (std::unique_ptr<Instance> &instance : population)
    {
        instance->cout(std::cout) << "\n";
    }

    return best;
}

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
std::unique_ptr<Instance> Argmax::evolution(std::function<std::unique_ptr<Instance>()> spawner, evolution_parameters parameters, bool show_best)
{
    auto rng = std::default_random_engine{};
    std::vector<InstanceGenWrapper> population = std::vector<InstanceGenWrapper>();
    population.reserve(parameters.population_max_size + parameters.population_spawn_size);
    for (unsigned int i = 0; i < parameters.population_start_size; i++)
        population.push_back(InstanceGenWrapper(spawner(), 0));

    std::cout << "\n";

    /* #region get best instance */
    std::unique_ptr<Instance> best = nullptr;
    float best_score = 0;
    int progress_percent = 0;
    for (InstanceGenWrapper &instance : population)
    {
        float score = instance.instance->score();

        if (score > best_score || best == nullptr)
        {
            best_score = score;
            best = instance.instance->clone();
            if (best->is_max_score(best_score))
                return best;
        }
    }
    /* #endregion */

    for (unsigned int g = 1; g < parameters.generation_count; g++)
    {
        /* #region spawning new generation */
        unsigned int current_spawn_count = 0;
        while (current_spawn_count < parameters.population_spawn_size)
        {
            std::shuffle(population.begin(), population.end(), rng);
            unsigned int parent_1 = 0;
            float score_1 = 0;
            unsigned int parent_2 = parameters.competition_goup_size;
            float score_2 = 0;

            // find parent 1
            for (unsigned int i = 0; i < parameters.competition_goup_size; i++)
            {
                unsigned int index = i % population.size();
                float score = population[index].instance->score();
                if (score > score_1)
                {
                    score_1 = score;
                    parent_1 = index;
                }
            }
            // find parent 2
            for (unsigned int i = parameters.competition_goup_size; i < parameters.competition_goup_size * 2; i++)
            {
                unsigned int index = i % population.size();
                float score = population[index].instance->score();
                if (score > score_2)
                {
                    score_2 = score;
                    parent_2 = index;
                }
            }

            // add child(s)
            for (unsigned int i = 0; i < parameters.spawn_per_parent && current_spawn_count < parameters.population_spawn_size; i++)
            {
                std::unique_ptr<Instance> instance = population[parent_1].instance->breed(population[parent_2].instance);

                // mutate offspring
                for (int arg = 0; arg < instance->nb_args(); arg++)
                    instance->mutate_arg(arg, parameters.mutation_probability);

                if (parameters.run_hc_on_child) instance = hill_climb(std::move(instance), 256);

                /* #region test for best instance */
                float score = instance->score();
                if (score > best_score)
                {
                    best_score = score;
                    best = instance->clone();
                    if (best->is_max_score(best_score))
                    {
                        std::cout << "\r\033[K";
                        return best;
                    }
                }
                /* #endregion */

                population.push_back(InstanceGenWrapper(std::move(instance), g));
                current_spawn_count++;
            }
        }
        /* #endregion */
        
        /* #region despawning part of the population */
        std::sort(population.begin(), population.end(),
            [](const InstanceGenWrapper &a, const InstanceGenWrapper &b)
            {
                return a.instance->score() < b.instance->score();
            });
        
        unsigned int despawned_count = 0;
        auto it = population.begin();

        while (it != population.end() && (despawned_count < parameters.population_despawn_size || population.size() > parameters.population_max_size))
        {
            if (parameters.protect_child_from_despawn && it->generation == g)
                it++;
            else {
                it = population.erase(it);
                despawned_count++;
            }
        }
        /* #endregion */
        
        /* #region pretty print to wait */
        int p = 100 * (float)g / parameters.generation_count;
        if (p != progress_percent)
        {
            progress_percent = p;
            std::string t = "\033[A\r\033[Kprogress: " + std::to_string(progress_percent);
            if (p < 10)
                t += " ";
            t += "% [";
            for (int i = 0; i < p / 10; i++)
                t += "="; //"▆";
            for (int i = p / 10; i < 10; i++)
                t += " ";
            
            t += "] it: " + std::to_string(g) + "/" + std::to_string(parameters.generation_count) + " \t";

            std::cout << t + "\n\033[K";
            if (show_best)
            {
                std::cout << "overall best ";
                best->cout(std::cout);
                std::cout << "\t with score of " + std::to_string(best_score);
            }
        }
        /* #endregion */
    }

    std::cout << "\r\033[K\033[A\r\033[K\r";

    return best;
}
