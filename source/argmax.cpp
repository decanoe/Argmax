#include "argmax.h"
#include <chrono>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>
#include <random>
#include <sstream>

float get_time_from(std::chrono::system_clock::time_point point)
{
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_time = end - point;
    double elapsed_seconds = elapsed_time.count();
    return elapsed_seconds;

    // auto start = std::chrono::system_clock::now();
    // float elapsed += get_time_from(start);
}

float Argmax::standard_derivation(std::vector<std::unique_ptr<Instance>> &population)
{
    std::vector<float> centroid = std::vector<float>();

    for (const auto &temp : population)
    {
        std::vector<float> vect = temp->to_normalized_point();
        while (centroid.size() < vect.size())
            centroid.push_back(0);

        for (size_t i = 0; i < vect.size(); i++)
            centroid[i] += vect[i];
    }
    for (size_t i = 0; i < centroid.size(); i++)
        centroid[i] /= population.size();

    float variance = 0;
    for (const auto &temp : population)
    {
        std::vector<float> vect = temp->to_normalized_point();

        for (size_t i = 0; i < vect.size(); i++)
        {
            variance += (centroid[i] - vect[i]) * (centroid[i] - vect[i]);
        }
    }
    return sqrtf(variance / population.size());
}
float Argmax::standard_derivation(std::vector<InstanceGenWrapper> &population)
{
    std::vector<float> centroid = std::vector<float>();

    for (const auto &temp : population)
    {
        std::vector<float> vect = temp.instance->to_normalized_point();
        while (centroid.size() < vect.size())
            centroid.push_back(0);

        for (size_t i = 0; i < vect.size(); i++)
            centroid[i] += vect[i];
    }
    for (size_t i = 0; i < centroid.size(); i++)
        centroid[i] /= population.size();

    float variance = 0;
    for (const auto &temp : population)
    {
        std::vector<float> vect = temp.instance->to_normalized_point();

        for (size_t i = 0; i < vect.size(); i++)
        {
            variance += (centroid[i] - vect[i]) * (centroid[i] - vect[i]);
        }
    }
    return sqrtf(variance / population.size());
}

/// @brief changes <instance> to its best scoring neighbor if it is better than itself
/// @param instance
/// @return true if a better neighbor was found
bool change_to_better_neighbor(std::unique_ptr<Instance> &instance)
{
    std::unique_ptr<Instance> best = nullptr;
    float score = instance->score();

    for (unsigned int i = 0; i < instance->nb_args(); i++)
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
/// @param black_list a list of the index of arguments not allowed to change
/// @return index of the argument changed (-1 if no neighbor exists)
int change_to_best_neighbor(std::unique_ptr<Instance> &instance, const std::list<int> &black_list)
{
    std::unique_ptr<Instance> best = nullptr;
    float score = instance->score();
    int index = -1;

    for (unsigned int i = 0; i < instance->nb_args(); i++)
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
std::unique_ptr<Instance> Argmax::tabu_search(const std::unique_ptr<Instance> start, size_t black_list_size, unsigned int max_iter)
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

std::unique_ptr<Instance> Argmax::one_lambda_search(const std::unique_ptr<Instance> start, unsigned int nb_mutation_to_test, unsigned int max_iter)
{
    std::unique_ptr<Instance> current = start->clone();
    std::unique_ptr<Instance> best = start->clone();
    float best_score = best->score();

    for (size_t i = 0; i < max_iter; i++)
    {
        std::unique_ptr<Instance> iter_best = nullptr;
        float score = 0;

        for (size_t j = 0; j < nb_mutation_to_test && j < current->nb_args(); j++)
        {
            std::unique_ptr<Instance> temp = current->clone();
            temp->mutate_arg(RandomUtils::get_index(current->nb_args()));
            float temp_score = temp->score();

            if (temp_score > score || iter_best == nullptr)
            {
                iter_best = std::move(temp);
                score = temp_score;

                if (iter_best->is_max_score(score))
                    return iter_best;
            }
        }

        current = std::move(iter_best);
        if (score > best_score)
        {
            best = current->clone();
            best_score = score;
        }
    }

    return best;
}

Argmax::evolution_parameters::evolution_parameters(const FileData &file_data)
{
    generation_count = file_data.get_int("generation_count", generation_count);

    population_start_size = file_data.get_int("population_start_size", population_start_size);
    population_spawn_size = file_data.get_int("population_spawn_size", population_spawn_size);
    population_despawn_size = file_data.get_int("population_despawn_size", population_despawn_size);

    spawn_per_parent = file_data.get_int("spawn_per_parent", spawn_per_parent);
    competition_goup_size = file_data.get_int("competition_goup_size", competition_goup_size);
    mutation_probability = file_data.get_float("mutation_probability", mutation_probability);

    /* #region run_algo_on_child */
    std::string algo = file_data.get_string("run_algo_on_child", "");
    if (algo == "hill_climb")
        run_algo_on_child = ChildAlgo::hill_climb;
    else if (algo == "tabu_search")
        run_algo_on_child = ChildAlgo::tabu_search;
    else if (algo == "lambda_mutation")
        run_algo_on_child = ChildAlgo::lambda_mutation;
    else
        run_algo_on_child = ChildAlgo::none;
    /* #endregion */
    child_algo_budget = file_data.get_int("child_algo_budget", child_algo_budget);
    child_algo_parameter = file_data.get_int("child_algo_parameter", child_algo_parameter);
    protect_child_from_despawn = file_data.get_bool("protect_child_from_despawn", protect_child_from_despawn);

    despawn_criteria_age_multiplier = file_data.get_float("despawn_criteria_age_multiplier", despawn_criteria_age_multiplier);
    despawn_criteria_diversity_multiplier = file_data.get_float("despawn_criteria_diversity_multiplier", despawn_criteria_diversity_multiplier);

    nb_islands = file_data.get_int("nb_islands", nb_islands);
    generation_between_migrations = file_data.get_int("generation_between_migrations", generation_between_migrations);
    percent_of_population_per_migrations = file_data.get_float("percent_of_population_per_migrations", percent_of_population_per_migrations);

    debug_show_best = file_data.get_bool("debug_show_best", debug_show_best);
    debug_generation_spacing = file_data.get_int("debug_generation_spacing", debug_generation_spacing);
}
std::ostream &Argmax::operator<<(std::ostream &c, const evolution_parameters &p)
{
    c << "generation_count:                         " << p.generation_count;

    c << "\npopulation_start_size:                    " << p.population_start_size;
    c << "\npopulation_spawn_size:                    " << p.population_spawn_size;
    c << "\npopulation_despawn_size:                  " << p.population_despawn_size;

    c << "\nspawn_per_parent:                         " << p.spawn_per_parent;
    c << "\ncompetition_goup_size:                    " << p.competition_goup_size;
    c << "\nmutation_probability:                     " << p.mutation_probability;

    c << "\nrun_algo_on_child:                        " << p.run_algo_on_child;
    c << "\nchild_algo_budget:                        " << p.child_algo_budget;
    c << "\nchild_algo_parameter:                     " << p.child_algo_parameter;
    c << "\nprotect_child_from_despawn:               " << p.protect_child_from_despawn;

    c << "\ndespawn_criteria_age_multiplier:          " << p.despawn_criteria_age_multiplier;
    c << "\ndespawn_criteria_diversity_multiplier:    " << p.despawn_criteria_diversity_multiplier;

    c << "\nnb_islands:                               " << p.nb_islands;
    c << "\ngeneration_between_migrations:            " << p.generation_between_migrations;
    c << "\npercent_of_population_per_migrations:     " << p.percent_of_population_per_migrations;

    c << "\ndebug_show_best:                          " << p.debug_show_best;
    c << "\ndebug_generation_spacing:                  " << p.debug_generation_spacing;
    return c;
}

void erase_lines(unsigned int &line_count)
{
    for (size_t i = 0; i < line_count; i++)
        std::cout << "\r\033[1A\033[K";
    line_count = 0;
}
float get_frequency_score(Instance* instance, const std::unique_ptr<std::vector<std::map<float, unsigned int>>>& arg_frequency_map, unsigned int population_size) {
    if (arg_frequency_map == nullptr) return 0;
    float result = 0;
    for (unsigned int i = 0; i < instance->nb_args(); i++)
    {
        float v = (*arg_frequency_map)[i][instance->get_coord(i)] / (float)population_size;
        result += v*v;
    }
    return result;
}
std::unique_ptr<Instance> Argmax::evolution(std::function<std::unique_ptr<Instance>()> spawner, evolution_parameters parameters, std::ofstream *out)
{
    /* #region output initialization */
    std::unique_ptr<std::stringstream> score_out = nullptr;
    std::unique_ptr<std::stringstream> population_out = nullptr;
    std::unique_ptr<std::stringstream> times_out = nullptr;
    if (out)
    {
        *out << parameters << "\n/*scores*/\n";

        score_out = std::make_unique<std::stringstream>();
        *score_out << "generation\tbest_score\tgen_best_score\tmean_score\tstd" << "\n";

        population_out = std::make_unique<std::stringstream>();
        *population_out << "generation\tage\tnb_args";

        std::unique_ptr<Instance> temp = spawner();

        for (unsigned int i = 0; i < temp->nb_args_max(); i++)
            *population_out << "\t" << temp->get_arg_labels(i);
        *population_out << "\n";
        
        times_out = std::make_unique<std::stringstream>();
        *times_out << "generation\tfinding_parent\tspawning_child\trunning_child_algo\tsorting_population\tdespawning\tfinding_best\ttotal" << "\n";
    }
    /* #endregion */

    /* #region initialization */
    auto rng = std::default_random_engine{};
    std::vector<InstanceGenWrapper> population = std::vector<InstanceGenWrapper>();
    
    population.reserve(parameters.population_start_size + parameters.population_spawn_size);
    for (unsigned int i = 0; i < parameters.population_start_size; i++)
        population.push_back(InstanceGenWrapper(spawner(), 0));

    std::unique_ptr<std::vector<std::map<float, unsigned int>>> arg_frequency_map = nullptr;
    if (parameters.despawn_criteria_diversity_multiplier != 0) {
        arg_frequency_map = std::make_unique<std::vector<std::map<float, unsigned int>>>(population[0].instance->nb_args_max(), std::map<float, unsigned int>());
        for (const auto &individual : population)
        {
            for (unsigned int i = 0; i < individual.instance->nb_args(); i++)
            {
                float v = individual.instance->get_coord(i);
                if ((*arg_frequency_map)[i].count(v) == 0)
                    (*arg_frequency_map)[i][v] = 1;
                else
                    (*arg_frequency_map)[i][v]++;
            }
        }
    }
    
    unsigned int line_count = 0;

    /* #region get best instance */
    std::unique_ptr<Instance> best = nullptr;
    float best_score = 0;
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
    /* #endregion */

    int progress_percent = -1;
    for (unsigned int g = 1; g < parameters.generation_count; g++)
    {
        auto start = std::chrono::system_clock::now();

        /* #region spawning new generation */
        float finding_parent_elapsed = 0;
        float spawning_child_elapsed = 0;
        float child_algo_elapsed = 0;
        unsigned int current_spawn_count = 0;
        while (current_spawn_count < parameters.population_spawn_size)
        {
            auto finding_parents_start = std::chrono::system_clock::now();
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
            finding_parent_elapsed += get_time_from(finding_parents_start);

            // add child(s)
            for (unsigned int i = 0; i < parameters.spawn_per_parent && current_spawn_count < parameters.population_spawn_size; i++)
            {
                auto spawning_child_start = std::chrono::system_clock::now();
                std::unique_ptr<Instance> instance = population[parent_1].instance->breed(population[parent_2].instance);

                // mutate offspring
                for (unsigned int arg = 0; arg < instance->nb_args(); arg++)
                    instance->mutate_arg(arg, parameters.mutation_probability);
                spawning_child_elapsed += get_time_from(spawning_child_start);
                
                auto child_algo_start = std::chrono::system_clock::now();
                switch (parameters.run_algo_on_child)
                {
                case evolution_parameters::ChildAlgo::hill_climb:
                    instance = hill_climb(std::move(instance), parameters.child_algo_budget / instance->nb_args());
                    break;
                case evolution_parameters::ChildAlgo::tabu_search:
                    instance = tabu_search(std::move(instance), parameters.child_algo_parameter, parameters.child_algo_budget / instance->nb_args());
                    break;
                case evolution_parameters::ChildAlgo::lambda_mutation:
                    instance = one_lambda_search(std::move(instance), parameters.child_algo_parameter, parameters.child_algo_budget / parameters.child_algo_parameter);
                    break;
                default:
                    break;
                }
                child_algo_elapsed += get_time_from(child_algo_start);

                auto adding_child_start = std::chrono::system_clock::now();
                /* #region test for best instance */
                float score = instance->score();
                if (score > best_score)
                {
                    best_score = score;
                    best = instance->clone();
                    if (best->is_max_score(best_score))
                    {
                        erase_lines(line_count);
                        return best;
                    }
                }
                /* #endregion */

                /* #region add instance to arg_frequency_map */
                if (arg_frequency_map) {
                    for (unsigned int i = 0; i < instance->nb_args(); i++)
                    {
                        float v = instance->get_coord(i);
                        if ((*arg_frequency_map)[i].count(v) == 0)
                            (*arg_frequency_map)[i][v] = 1;
                        else
                            (*arg_frequency_map)[i][v]++;
                    }
                }
                /* #endregion */

                population.push_back(InstanceGenWrapper(std::move(instance), g));
                current_spawn_count++;
                spawning_child_elapsed += get_time_from(adding_child_start);
            }
        }
        /* #endregion */
        
        /* #region despawning part of the population */
        auto sorting_start = std::chrono::system_clock::now();
        std::vector<unsigned int> remove_indices = std::vector<unsigned int>();
        std::vector<float> remove_scores = std::vector<float>();
        remove_indices.reserve(parameters.population_despawn_size);
        remove_scores.reserve(parameters.population_despawn_size);

        float gen_mult = parameters.despawn_criteria_age_multiplier;
        float freq_mult = -parameters.despawn_criteria_diversity_multiplier;
        for (unsigned int i = 0; i < population.size(); i++)
        {
            if (parameters.protect_child_from_despawn && population[i].generation == g) continue;

            float score = population[i].instance->score();
            score += get_frequency_score(population[i].instance.get(), arg_frequency_map, population.size()) * freq_mult;
            score += population[i].generation * gen_mult;

            auto score_iter = remove_scores.begin();
            auto iter = remove_indices.begin();
            while (iter != remove_indices.end()) {
                if (score < *score_iter) {
                    remove_indices.insert(iter, i);
                    remove_scores.insert(score_iter, score);
                    break;
                }
                score_iter++;
                iter++;
            }

            if (remove_indices.size() < parameters.population_despawn_size) {
                remove_indices.push_back(i);
                remove_scores.push_back(score);
            }
            else if (remove_indices.size() > parameters.population_despawn_size) {
                remove_indices.pop_back();
                remove_scores.pop_back();
            }
        }
        
        float sorting_elapsed = get_time_from(sorting_start);

        auto despawning_start = std::chrono::system_clock::now();
        for (unsigned int index = 0; index < remove_indices.size(); index++)
        {
            /* #region remove instance to arg_frequency_map */
            if (arg_frequency_map) {
                for (unsigned int i = 0; i < population[remove_indices[index]].instance->nb_args(); i++)
                    (*arg_frequency_map)[i][population[remove_indices[index]].instance->get_coord(i)]--;
            }
            /* #endregion */
            population.erase(population.begin() + remove_indices[index]);

            for (unsigned int i = index + 1; i < remove_indices.size(); i++) if (remove_indices[i] > remove_indices[index]) remove_indices[i]--;
        }
        float despawning_elapsed = get_time_from(despawning_start);
        /* #endregion */
        
        /* #region get best instance in gen */
        auto searching_start = std::chrono::system_clock::now();
        unsigned int best_in_gen = population.size();
        float best_score_in_gen = 0;
        float mean_score = 0;
        for (size_t i = 0; i < population.size(); i++)
        {
            float score = population[i].instance->score();
            mean_score += score;
            if (score > best_score_in_gen || best_in_gen == population.size())
            {
                best_score_in_gen = score;
                best_in_gen = i;
            }
        }
        mean_score /= population.size();
        float searching_elapsed = get_time_from(searching_start);
        /* #endregion */
        
        float elapsed = get_time_from(start);

        /* #region fill output file */
        float gen_standard_derivation = standard_derivation(population);
        if (out && (g % parameters.debug_generation_spacing == 0))
        {
            *score_out << g;
            *score_out << "\t" << best_score;
            *score_out << "\t" << best_score_in_gen;
            *score_out << "\t" << mean_score;
            *score_out << "\t" << gen_standard_derivation;
            *score_out << "\n";

            for (InstanceGenWrapper &instance : population)
            {
                *population_out << g;
                *population_out << "\t" << g - instance.generation;

                auto point = instance.instance->to_debug_point();
                *population_out << "\t" << point.size();
                for (const std::string &v : point)
                    *population_out << "\t" << v;

                for (unsigned int i = point.size(); i < instance.instance->nb_args_max(); i++)
                    *population_out << "\t";
                *population_out << "\n";
            }

            *times_out << g;
            *times_out << "\t" << finding_parent_elapsed;
            *times_out << "\t" << spawning_child_elapsed;
            *times_out << "\t" << child_algo_elapsed;
            *times_out << "\t" << sorting_elapsed;
            *times_out << "\t" << despawning_elapsed;
            *times_out << "\t" << searching_elapsed;
            *times_out << "\t" << elapsed;
            *times_out << "\n";
        }
        /* #endregion */
        
        /* #region pretty print to wait */
        int p = 100 * (float)g / parameters.generation_count;
        if (p != progress_percent)
        {
            erase_lines(line_count);
            progress_percent = p;
            std::string t = "progress: " + std::to_string(progress_percent);
            if (p < 10)
                t += " ";
            t += "% [";
            for (int i = 0; i < p / 10; i++)
                t += "="; //"▆";
            for (int i = p / 10; i < 10; i++)
                t += " ";

            t += "] it: " + std::to_string(g) + "/" + std::to_string(parameters.generation_count) + "\n";
            std::cout << t;
            line_count++;

            if (parameters.debug_show_best)
            {
                std::cout << "best in generation ";
                population[best_in_gen].instance->cout(std::cout);
                std::cout << "\t with score of " + std::to_string(best_score_in_gen) + "\tAge: " + std::to_string(g - population[best_in_gen].generation) + "\n";
                line_count++;

                std::cout << "overall best       ";
                best->cout(std::cout);
                std::cout << "\t with score of " + std::to_string(best_score) + "\n";
                line_count++;
            }

            std::cout << "standard deviation: " + std::to_string(gen_standard_derivation) + "\n";
            line_count++;
        }
        /* #endregion */
    }

    erase_lines(line_count);

    if (out) *out
            << score_out->str()         << "/*populations*/\n"
            << population_out->str()    << "/*times*/\n"
            << times_out->str()    << "/*times*/\n";
    return best;
}
