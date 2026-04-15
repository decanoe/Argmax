#include "hill_climber.h"

LocalSearch::HillClimber::Selection_Criterion LocalSearch::HillClimber::get_Selection_Criterion(const std::string& string) {
    if (string == "random") return Selection_Criterion::Random;
    else if (string == "first") return Selection_Criterion::First;
    else if (string == "cycle") return Selection_Criterion::Cycle;
    else if (string == "least") return Selection_Criterion::Least;
    else if (string == "best") return Selection_Criterion::Best;
    return Selection_Criterion::First;
}

unsigned int LocalSearch::HillClimber::get_test_index(unsigned int default_index, unsigned int iteration_index, const std::set<unsigned int>& visited_indices, unsigned int nb_args) {
    switch (this->criterion)
    {
    case LocalSearch::HillClimber::Selection_Criterion::Random: return RandomUtils::get_index(nb_args, visited_indices, this->random_generator);
    case LocalSearch::HillClimber::Selection_Criterion::Cycle: return (default_index + iteration_index) % nb_args;
    default: return default_index;
    }
}
bool LocalSearch::HillClimber::do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score) {
    switch (this->criterion)
    {
    case LocalSearch::HillClimber::Selection_Criterion::Least: return (tested_score > init_score) && (first_keep || (tested_score < iteration_best_score));
    case LocalSearch::HillClimber::Selection_Criterion::Best: return (tested_score > init_score) && (first_keep || (tested_score > iteration_best_score));
    default: return (tested_score > init_score);
    }
}
bool LocalSearch::HillClimber::stop_at_first_improve() {
    switch (this->criterion)
    {
    case LocalSearch::HillClimber::Selection_Criterion::Least: return false;
    case LocalSearch::HillClimber::Selection_Criterion::Best: return false;
    default: return true;
    }
}
unsigned int LocalSearch::HillClimber::improve(std::unique_ptr<ReversibleInstance>& instance, unsigned int budget, unsigned int initial_budget) {
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    unsigned int better_neighbors_after = better_neighbors;
    output_iteration_ends_data(used_budget, (used_budget  - initial_budget), better_neighbors, score);
    
    unsigned int iteration_count = 0;
    while (used_budget < budget)
    {
        unsigned int iteration_best_move = -1U;
        float iteration_best_score = 0;

        // iterate through mutations
        std::set<unsigned int> visited = std::set<unsigned int>();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            unsigned int index = get_test_index(i, iteration_count, visited, instance->nb_args());
            visited.insert(index);

            instance->mutate_arg(index);
            used_budget++;
            if (do_keep(instance->score(), score, iteration_best_move == -1U, iteration_best_score)) {
                iteration_best_move = index;
                iteration_best_score = instance->score();
                if (stop_at_first_improve()) { instance->revert_last_mutation(); break; }
            }
            instance->revert_last_mutation();
        }

        // apply best mutation
        if (iteration_best_move != -1U) {
            float old_score = instance->score();
            instance->mutate_arg(iteration_best_move);
            better_neighbors_after = count_better_neighbors(instance);
            output_iteration_data(used_budget, (used_budget  - initial_budget), better_neighbors, better_neighbors_after, old_score, iteration_best_score, 1);
            better_neighbors = better_neighbors_after;
            score = iteration_best_score;
        }
        else {
            output_iteration_ends_data(used_budget, (used_budget  - initial_budget), better_neighbors, score);
            break;
        }
        iteration_count++;
    }

    return used_budget;
}

LocalSearch::HillClimber::HillClimber(unsigned int seed, Selection_Criterion criterion, bool debug): LocalSearch::LocalSearchAlgo(seed, debug), criterion(criterion) {}
LocalSearch::HillClimber::HillClimber(unsigned int seed, Selection_Criterion criterion, bool debug, std::ostream* out, bool add_header): LocalSearch::LocalSearchAlgo(seed, debug, out, add_header), criterion(criterion) {}

