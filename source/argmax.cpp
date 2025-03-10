#include "argmax.h"
#include <chrono>
#include <list>
#include <vector>
#include <algorithm>
#include <string>

float get_time_from(std::chrono::system_clock::time_point point) {
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_time = end-point;
    double elapsed_seconds = elapsed_time.count();
    return elapsed_seconds;


    // auto start = std::chrono::system_clock::now();
    // float elapsed += get_time_from(start);
}

/// @brief changes <instance> to its best scoring neighbor
/// @param instance 
/// @return true if a better neighbor was found
bool change_to_better_neighbor(std::unique_ptr<Instance>& instance) {
    std::unique_ptr<Instance> best = nullptr;
    float score = instance->score();
    
    for (int i = 0; i < instance->nb_args(); i++)
    {
        std::unique_ptr<Instance> mutation = instance->clone();
        mutation->mutate_arg(i);
        float mutation_score = mutation->score();
        
        if (mutation_score > score) {
            best = std::move(mutation);
            score = mutation_score;
        }
    }
    
    if (best == nullptr) return false;
    instance = std::move(best);
    return true;
}
std::unique_ptr<Instance> Argmax::hill_climb(const std::unique_ptr<Instance> start, unsigned int max_iter) {
    std::unique_ptr<Instance> result = start->clone();
    unsigned int i = 0;
    while (change_to_better_neighbor(result) && ++i < max_iter);
    return result;
}

/// @brief changes <instance> to its best scoring neighbor
/// @param instance 
/// @return index of the argument changed (-1 if max was reach)
int change_to_best_neighbor(std::unique_ptr<Instance>& instance, const std::list<int>& black_list) {
    std::unique_ptr<Instance> best = nullptr;
    float score = instance->score();
    int index = -1;
    
    for (int i = 0; i < instance->nb_args(); i++)
    {
        if (std::find(black_list.begin(), black_list.end(), i) != black_list.end()) continue;
        
        std::unique_ptr<Instance> mutation = instance->clone();
        mutation->mutate_arg(i);
        float mutation_score = mutation->score();
        
        if (mutation_score > score || index == -1) {
            index = i;
            best = std::move(mutation);
            score = mutation_score;
            
            if (best->is_max_score(score)) {
                instance = std::move(best);
                return -1;
            }
        }
    }
    
    instance = std::move(best);
    return index;
}
std::unique_ptr<Instance> Argmax::hill_climb_tab(const std::unique_ptr<Instance> start, size_t black_list_size, unsigned int max_iter) {
    std::list<int> black_list;
    std::unique_ptr<Instance> result = start->clone();
    std::unique_ptr<Instance> best = result->clone();
    for (size_t i = 0; i < max_iter; i++)
    {
        int index = change_to_best_neighbor(result, black_list);
        if (index == -1) return result;
        
        if (result->score() > best->score()) best = result->clone();

        black_list.push_front(index);
        if (black_list.size() > black_list_size) black_list.pop_back();
    }
    
    return best;
}




std::unique_ptr<Instance> Argmax::simple_evolution(std::function<std::unique_ptr<Instance>()> spawner, simple_evolution_parameters parameters) {
    std::vector<std::unique_ptr<Instance>> population = std::vector<std::unique_ptr<Instance>>();
    population.reserve(parameters.population_size);
    for (unsigned int i = 0; i < parameters.population_size; i++) population.push_back(spawner());
    
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
        for (std::unique_ptr<Instance>& instance : population)
        {
            float score = instance->score();

            if (score > best_score || best == nullptr) {
                best_score = score;
                best = instance->clone();
                if (best->is_max_score(best_score)) {
                    std::cout << "\r                                                                                         \r";
                    return best;
                }
            }

            if (score > score_1 || parent_1 == nullptr) {
                score_1 = score;
                parent_1 = std::move(instance);
            }
            else if (score > score_2 || parent_2 == nullptr) {
                score_2 = score;
                parent_2 = std::move(instance);
            }
        }

        if (parent_1 == nullptr || parent_2 == nullptr) {
            std::cerr << "\033[1;31mNot enough instances in the population, can't find parents !\033[0m";
            exit(-1);
        }
        
        // make new population
        for (unsigned int i = 0; i < parameters.population_size; i++) {
            population[i] = parent_1->breed(parent_2);
            
            //mutate offspring
            for (int arg = 0; arg < population[i]->nb_args(); arg++) population[i]->mutate_arg(arg, parameters.mutation_probability);
        }




        // pretty print to wait
        int p = 100 * (float)g / parameters.generation_count;
        if (p != progress_percent) {
            progress_percent = p;
            std::string t = "\rprogress: " + std::to_string(progress_percent);
            if (p < 10) t += " ";
            t += "% [";
            for (int i = 0; i < p / 10; i++) t += "=";//"▆";
            for (int i = p / 10; i < 10; i++) t += " ";
            
            std::cout << t + "] best in generation: " + std::to_string(score_1) + "\t overall best: " + std::to_string(best_score) + "                    ";
        }
    }
    
    std::cout << "\r                                                                                           \r";
    return best;
}





