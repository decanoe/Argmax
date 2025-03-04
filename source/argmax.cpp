#include "argmax.h"

/// @brief changes <instance> to its best scoring neighbor
/// @param instance 
/// @return true if a better neighbor was found
bool change_to_best_neighbor(std::unique_ptr<Instance>& instance) {
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

std::unique_ptr<Instance> hill_climb(const std::unique_ptr<Instance> start) {
    std::unique_ptr<Instance> result = start->clone();
    while (change_to_best_neighbor(result));
    return result;
}