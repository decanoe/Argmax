#include "one_lambda.h"
#include "../../greedy_jumper/greedy_jumper.h"

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

/* #region ======================================= GreedyTabuSearch ======================================= */
GreedyOneLambdaSearch::GreedyOneLambdaSearch(float lambda, bool enable_aspiration): OneLambdaSearch(lambda, enable_aspiration) {}

bool GreedyOneLambdaSearch::improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget, float aspiration_score) {
    unsigned int old_improving_neighbor_count = improving_neighbor_count;
    float old_score = score;

    // create trajectory
    GreedyJumper::TrajectorySet trajectory(GreedyJumper::cmp);
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
        if (i < instance->nb_args() * lambda) trajectory.insert({index, instance->score()});
        // add to the trajectory if aspiration will force it to be chosed
        else if (this->aspiration && instance->score() > aspiration_score) trajectory.insert({i, instance->score()});
        instance->revert_last_mutation();
    }
    // chose aspiration if it occured
    if (this->aspiration && trajectory.begin()->second > aspiration_score) {
        score = instance->score();
        improving_neighbor_count = count_better_neighbors(instance);
        output_iteration_data(budget, old_improving_neighbor_count, improving_neighbor_count, old_score, score, 1);
        return true;
    }

    // apply all mutations in trajectory
    for (std::pair<unsigned int, float> pair : trajectory) instance->mutate_arg(pair.first);
    
    // compare jumps of the trajectory
    unsigned int count = trajectory.size();
    unsigned int best_count = -1U;
    float best_score = 0;
    for (auto pair = trajectory.rbegin(); pair != trajectory.rend(); pair++) {
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
        instance->mutate_arg(pair->first);
    }
    if (best_count == -1U) {
        output_iteration_ends_data(budget, improving_neighbor_count, score);
        return best_count != 0;
    }

    // apply best jump found
    score = best_score;
    count = best_count;
    for (std::pair<unsigned int, float> pair : trajectory) {
        if (count == 0) break;
        count--;
        instance->mutate_arg(pair.first);
    }
    
    improving_neighbor_count = count_better_neighbors(instance);
    output_iteration_data(budget, old_improving_neighbor_count, improving_neighbor_count, old_score, score, best_count);
    return true;
}
/* #endregion */
