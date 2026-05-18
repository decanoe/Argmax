#include "one_lambda.h"

using namespace LocalSearch;

/* #region ======================================= OneLambdaSearch ======================================= */
OneLambdaSearch::OneLambdaSearch(float lambda, bool enable_aspiration): NonIteratedLocalSearch(enable_aspiration), lambda(lambda) {}

bool OneLambdaSearch::improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget, float aspiration_score) {
    float old_score = score;
    unsigned int old_improving_neighbor_count = improving_neighbor_count;

    unsigned int iteration_best_move = -1U;
    float iteration_best_score = 0;

    // iterate through mutations
    std::set<unsigned int> visited_indices = std::set<unsigned int>();
    for (size_t i = 0; i < instance->nb_args() * lambda || (this->aspiration && i < instance->nb_args()); i++)
    {
        if (budget.out_of_budget()) {
            output_iteration_ends_data(budget, improving_neighbor_count, score);
            return false;
        }

        unsigned int index = RandomUtils::get_index(instance->nb_args(), visited_indices, *this->random_generator);
        visited_indices.insert(index);

        instance->mutate_arg(index);
        budget++;
        if (iteration_best_move == -1U || instance->score() > iteration_best_score) {
            if (i < instance->nb_args() * lambda || (this->aspiration && instance->score() > aspiration_score)) {
                iteration_best_move = index;
                iteration_best_score = instance->score();
            }
        }
        instance->revert_last_mutation();
    }

    if (iteration_best_move == -1U) {
        output_iteration_ends_data(budget, improving_neighbor_count, score);
        return false;
    }

    // apply best mutation
    instance->mutate_arg(iteration_best_move);
    improving_neighbor_count = count_better_neighbors(instance);
    score = iteration_best_score;
    output_iteration_data(budget, old_improving_neighbor_count, improving_neighbor_count, old_score, score, 1);
    return true;
}
/* #endregion */

/* #region ======================================= GreedyOneLambdaSearch ======================================= */
GreedyOneLambdaSearch::GreedyOneLambdaSearch(float lambda, bool enable_aspiration, GreedyTrajectory::NeighborhoodOrdering positive_ordering, GreedyTrajectory::NeighborhoodOrdering negative_ordering): OneLambdaSearch(lambda, enable_aspiration), trajectory(positive_ordering, negative_ordering) {}

bool GreedyOneLambdaSearch::improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget, float aspiration_score) {
    unsigned int old_improving_neighbor_count = improving_neighbor_count;
    float old_score = score;
    unsigned int aspiration_flip = -1U;

    // create trajectory
    trajectory.clear();
    std::set<unsigned int> visited_indices = std::set<unsigned int>();
    for (size_t i = 0; i < instance->nb_args() * lambda || (this->aspiration && i < instance->nb_args()); i++)
    {
        if (budget.out_of_budget()) {
            output_iteration_ends_data(budget, improving_neighbor_count, score);
            return false;
        }
        unsigned int index = RandomUtils::get_index(instance->nb_args(), visited_indices, *this->random_generator);
        visited_indices.insert(index);

        instance->mutate_arg(index);
        budget++;
        if (this->aspiration && instance->score() > aspiration_score) {
            aspiration_flip = index;
            aspiration_score = instance->score();
        }
        if (i < instance->nb_args() * lambda) {
            if (instance->score() > score)
                trajectory.insert_positive_flip(BitFlip(index, instance->score()));
            else
                trajectory.insert_negative_flip(BitFlip(index, instance->score()));
        }
        instance->revert_last_mutation();
    }
    trajectory.finalize();

    // apply all mutations in trajectory
    for (const BitFlip& bitflip : trajectory) instance->mutate_arg(bitflip.index);
    
    // compare jumps of the trajectory
    unsigned int count = trajectory.size();
    unsigned int best_count = -1U;
    float best_score = 0;
    for (auto bitflip = trajectory.rbegin(); bitflip != trajectory.rend(); ++bitflip) {
        if (budget.out_of_budget()) {
            output_iteration_ends_data(budget, improving_neighbor_count, score);
            return false;
        }

        budget++;
        if (best_count == -1U || instance->score() > best_score) {
            best_score = instance->score();
            best_count = count;
        }
        count--;
        instance->mutate_arg((*bitflip).index);
    }
    if (best_count == -1U && aspiration_flip == -1U) {
        output_iteration_ends_data(budget, improving_neighbor_count, score);
        return false;
    }
    // chose aspiration if it occured
    if (this->aspiration && aspiration_flip != -1U && aspiration_score > best_score) {
        instance->mutate_arg(aspiration_flip);
        score = aspiration_score;
        improving_neighbor_count = count_better_neighbors(instance);
        output_iteration_data(budget, old_improving_neighbor_count, improving_neighbor_count, old_score, score, 1);
        return true;
    }

    // apply best jump found
    score = best_score;
    count = best_count;
    for (const BitFlip& bitflip : trajectory) {
        if (count == 0) break;
        count--;
        instance->mutate_arg(bitflip.index);
    }
    
    improving_neighbor_count = count_better_neighbors(instance);
    output_iteration_data(budget, old_improving_neighbor_count, improving_neighbor_count, old_score, score, best_count);
    return true;
}
/* #endregion */
