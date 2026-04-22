#include "tabu.h"
#include <queue>

using namespace LocalSearch::Tabu;



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
TabuSearch::TabuSearch(std::shared_ptr<Selection_Criterion> criterion, float tabu_size, float max_random_size_added): HillClimber(criterion), tabu_size(tabu_size), tabu_max_random_size_added(max_random_size_added) {}

unsigned int TabuSearch::improve(std::unique_ptr<ReversibleInstance>& instance, unsigned int budget, unsigned int initial_budget) const {
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    unsigned int better_neighbors_after = better_neighbors;
    output_iteration_ends_data(used_budget, (used_budget  - initial_budget), better_neighbors, score);
    
    TabuList tabu_list(this->tabu_size * instance->nb_args(), this->tabu_max_random_size_added * instance->nb_args(), this->random_generator);
    unsigned int iteration_count = 0;
    while (used_budget < budget)
    {
        unsigned int iteration_best_move = -1U;
        float iteration_best_score = 0;
        tabu_list.randomize_size();

        // iterate through mutations
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            unsigned int index = criterion->get_test_index(i, iteration_count, instance->nb_args());
            if (tabu_list.contains(index)) continue;

            instance->mutate_arg(index);
            used_budget++;
            if (criterion->do_keep(instance->score(), score, iteration_best_move == -1U, iteration_best_score)) {
                iteration_best_move = index;
                iteration_best_score = instance->score();
                if (criterion->stop_at_first_improve()) { instance->revert_last_mutation(); break; }
            }
            instance->revert_last_mutation();
        }

        // apply best mutation
        if (iteration_best_move != -1U) {
            tabu_list.push(iteration_best_move);

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
/* #endregion */

/* #region ======================================= GreedyTabuSearch ======================================= */
GreedyTabuSearch::GreedyTabuSearch(std::shared_ptr<Selection_Criterion> criterion, std::shared_ptr<Neighborhood_Scope> scope, float tabu_size, float max_random_size_added, TabuPushOrder tabu_push_order): GreedyJumper(criterion, scope), tabu_push_order(tabu_push_order), tabu_size(tabu_size), tabu_max_random_size_added(max_random_size_added) {}
GreedyTabuSearch::TabuPushOrder GreedyTabuSearch::push_order_from_string(const std::string& string) {
    if (string == "BestToWorst") return TabuPushOrder::BestToWorst;
    if (string == "BestToWorstClamped") return TabuPushOrder::BestToWorstClamped;
    if (string == "WorstToBest") return TabuPushOrder::WorstToBest;
    return TabuPushOrder::BestToWorstClamped;
}

unsigned int GreedyTabuSearch::improve(std::unique_ptr<ReversibleInstance>& instance, unsigned int budget, unsigned int initial_budget) const {
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    unsigned int better_neighbors_after = better_neighbors;
    output_iteration_ends_data(used_budget, (used_budget  - initial_budget), better_neighbors, score);

    TabuList tabu_list(this->tabu_size * instance->nb_args(), this->tabu_max_random_size_added * instance->nb_args(), this->random_generator);
    GreedyJumper::TrajectorySet trajectory(cmp);
    this->scope->reset();
    while (used_budget < budget)
    {
        tabu_list.randomize_size();
        // create trajectory
        // used_budget += this->scope->create_trajectory(trajectory, instance, score, [&tabu_list](unsigned int i){ return tabu_list.contains(i); });

        // apply all mutations in trajectory
        for (std::pair<unsigned int, float> pair : trajectory) instance->mutate_arg(pair.first);

        // compare jumps of the trajectory
        unsigned int count = trajectory.size();
        unsigned int iteration_best_count = 0;
        float iteration_best_score = score;
        for (auto pair = trajectory.rbegin(); pair != trajectory.rend(); pair++) {
            used_budget++;
            if (this->criterion->do_keep(instance->score(), score, iteration_best_count == 0, iteration_best_score)) {
                iteration_best_score = instance->score();
                iteration_best_count = count;
                if (this->criterion->stop_at_first_improve()) break;
            }
            count--;
            instance->mutate_arg(pair->first);
        }

        // apply best jump found
        float old_score = score;
        score = iteration_best_score;
        if (!this->criterion->stop_at_first_improve()) {
            count = iteration_best_count;
            for (std::pair<unsigned int, float> pair : trajectory) {
                if (count == 0) break;
                count--;
                instance->mutate_arg(pair.first);
            }
        }

        // add flipped variables to the tabu list
        std::vector<unsigned int> flipped = std::vector<unsigned int>();
        count = iteration_best_count;
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
        
        better_neighbors_after = count_better_neighbors(instance);
        output_iteration_data(used_budget, (used_budget  - initial_budget), better_neighbors, better_neighbors_after, old_score, score, iteration_best_count);
        better_neighbors = better_neighbors_after;
        if (iteration_best_count == 0) return used_budget;
    }
    return used_budget;
}
/* #endregion */
