#include "argmax.h"
#include <chrono>
#include <list>
#include <algorithm>

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
std::unique_ptr<Instance> hill_climb(const std::unique_ptr<Instance> start, unsigned int max_iter) {
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
std::unique_ptr<Instance> hill_climb_tab(const std::unique_ptr<Instance> start, size_t black_list_size, unsigned int max_iter) {
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