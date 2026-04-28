#include "non_iterated_local_search.h"

using namespace LocalSearch;

NonIteratedLocalSearch::NonIteratedLocalSearch(bool enable_aspiration): LocalSearchAlgo(), aspiration(enable_aspiration) {}

float NonIteratedLocalSearch::run(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) {
    float score = instance->score();
    float best_score = score;
    std::unique_ptr<ReversibleInstance> best_instance = instance->clone();
    budget++;
    unsigned int better_neighbors = count_better_neighbors(instance);
    output_iteration_ends_data(budget, better_neighbors, score);
    
    while (!budget.out_of_budget() && this->improve(instance, score, better_neighbors, budget, best_score)) {
        if (score > best_score) {
            best_score = score;
            best_instance = instance->clone();
        }
    }

    instance = std::move(best_instance);
    return score;
}

bool NonIteratedLocalSearch::improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget) {
    std::cerr << "NonIteratedLocalSearch::improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget) should not be used !" << std::endl;
    exit(1);
}
