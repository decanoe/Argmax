#include "tabu.h"

using namespace LocalSearch;

/* #region ======================================= TabuList ======================================= */
TabuList::TabuList(size_t size): default_size(size), current_size(size), random_max(0), tabu_queue(), random_generator(nullptr) { }
TabuList::TabuList(size_t default_size, size_t max_random_size_added, std::shared_ptr<std::mt19937> random_generator): default_size(default_size), current_size(default_size), random_max(max_random_size_added), tabu_queue(), random_generator(random_generator) { }
void TabuList::randomize_size() {
    if (random_max > 0) this->current_size = this->default_size + RandomUtils::get_index(random_max, *random_generator);
}
void TabuList::push(unsigned int value) {
    if (tabu_queue.size() == max_size()) tabu_queue.pop_front();
    tabu_queue.push_back(value);
}

bool TabuList::contains(unsigned int value) const {
    for (size_t i = 0; i < current_size && i < tabu_queue.size(); ++i) {
        if (tabu_queue[i] == value)
            return true;
    }
    return false;
}

size_t TabuList::max_size() const { return default_size + random_max; }
/* #endregion */

/* #region ======================================= TabuSearch ======================================= */
TabuSearch::TabuSearch(float tabu_size, float max_random_size_added, bool enable_aspiration): NonIteratedLocalSearch(enable_aspiration), tabu_size(tabu_size), tabu_max_random_size_added(max_random_size_added), tabu_list(0) {}

float TabuSearch::run(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) {
    this->tabu_list = TabuList(this->tabu_size * instance->nb_args(), this->tabu_max_random_size_added * instance->nb_args(), this->random_generator);
    
    return NonIteratedLocalSearch::run(instance, budget);
}
bool TabuSearch::improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget, float aspiration_score) {
    this->tabu_list.randomize_size();
    float old_score = score;
    unsigned int old_improving_neighbor_count = improving_neighbor_count;

    unsigned int iteration_best_move = -1U;
    float iteration_best_score = 0;

    // iterate through mutations
    for (size_t i = 0; i < instance->nb_args(); i++)
    {
        if (!this->aspiration && this->tabu_list.contains(i)) continue;
        if (budget.out_of_budget()) {
            output_iteration_ends_data(budget, improving_neighbor_count, score);
            return false;
        }

        instance->mutate_arg(i);
        budget++;
        if (iteration_best_move == -1U || instance->score() > iteration_best_score) {
            if (!this->tabu_list.contains(i) || (this->aspiration && instance->score() > aspiration_score)) {
                iteration_best_move = i;
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
    tabu_list.push(iteration_best_move);
    improving_neighbor_count = count_better_neighbors(instance);
    score = iteration_best_score;
    output_iteration_data(budget, old_improving_neighbor_count, improving_neighbor_count, old_score, score, 1);
    return true;
}
/* #endregion */

/* #region ======================================= GreedyTabuSearch ======================================= */
GreedyTabuSearch::GreedyTabuSearch(float tabu_size, float max_random_size_added, TabuPushOrder tabu_push_order, bool enable_aspiration): TabuSearch(tabu_size, max_random_size_added, enable_aspiration), tabu_push_order(tabu_push_order) {}
GreedyTabuSearch::TabuPushOrder GreedyTabuSearch::push_order_from_string(const std::string& string) {
    if (string == "BestToWorst") return TabuPushOrder::BestToWorst;
    if (string == "BestToWorstClamped") return TabuPushOrder::BestToWorstClamped;
    if (string == "WorstToBest") return TabuPushOrder::WorstToBest;
    if (string == "Random") return TabuPushOrder::Random;
    return TabuPushOrder::BestToWorstClamped;
}

void GreedyTabuSearch::tabu_push(const GreedyJumper::TrajectorySet& trajectory, unsigned int jump_size) {
    std::vector<unsigned int> flipped = std::vector<unsigned int>();
    unsigned int count = jump_size;
    if (tabu_push_order == TabuPushOrder::BestToWorstClamped) count = std::max(count, (unsigned int)tabu_list.max_size());
    for (std::pair<unsigned int, float> pair : trajectory) {
        if (count == 0) break;
        count--;
        flipped.push_back(pair.first);
    }


    if (tabu_push_order == TabuPushOrder::BestToWorst || tabu_push_order == TabuPushOrder::BestToWorstClamped)
        for (unsigned int v : flipped) tabu_list.push(v);
    else if (tabu_push_order == TabuPushOrder::WorstToBest)
        for (auto v = flipped.rbegin(); v != flipped.rend(); v++) tabu_list.push(*v);
    else if (tabu_push_order == TabuPushOrder::Random)
    {
        while (flipped.size() != 0)
        {
            unsigned int index = RandomUtils::get_index(flipped.size(), *this->random_generator);
            tabu_list.push(flipped[index]);

            flipped[index] = flipped[flipped.size() - 1];
            flipped.pop_back();
        }
    }
}
bool GreedyTabuSearch::improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget, float aspiration_score) {
    unsigned int old_improving_neighbor_count = improving_neighbor_count;
    float old_score = score;

    tabu_list.randomize_size();
    // create trajectory
    GreedyJumper::TrajectorySet trajectory(GreedyJumper::cmp);
    for (size_t i = 0; i < instance->nb_args(); i++)
    {
        if (!this->aspiration && this->tabu_list.contains(i)) continue;
        if (budget.out_of_budget()) {
            output_iteration_ends_data(budget, improving_neighbor_count, score);
            return false;
        }

        instance->mutate_arg(i);
        budget++;
        if (!this->tabu_list.contains(i)) trajectory.insert({i, instance->score()});
        // add to the trajectory if aspiration will force it to be chosed
        else if (this->aspiration && instance->score() > aspiration_score) trajectory.insert({i, instance->score()});
        instance->revert_last_mutation();
    }
    // chose aspiration if it occured
    if (this->aspiration && trajectory.begin()->second > aspiration_score) {
        instance->mutate_arg(trajectory.begin()->first);
        tabu_list.push(trajectory.begin()->first);
        score = trajectory.begin()->second;
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
    tabu_push(trajectory, best_count);
    
    improving_neighbor_count = count_better_neighbors(instance);
    output_iteration_data(budget, old_improving_neighbor_count, improving_neighbor_count, old_score, score, best_count);
    return true;
}
/* #endregion */
