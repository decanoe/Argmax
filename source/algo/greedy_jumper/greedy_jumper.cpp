#include "greedy_jumper.h"

auto cmp = [](std::pair<unsigned int, float> a, std::pair<unsigned int, float> b) {
    if (a.second != b.second) return a.second > b.second;
    else return a.first < b.first;
};

LocalSearch::GreedyJumper::Selection_Criterion LocalSearch::GreedyJumper::get_Selection_Criterion(const std::string& string) {
    if (string == "first") return Selection_Criterion::First;
    else if (string == "best") return Selection_Criterion::Best;
    else if (string == "least") return Selection_Criterion::Least;
    return Selection_Criterion::First;
}
LocalSearch::GreedyJumper::Neighborhood_Scope LocalSearch::GreedyJumper::get_Neighborhood_Scope(const std::string& string) {
    if (string == "improve") return Neighborhood_Scope::Improve;
    else if (string == "all") return Neighborhood_Scope::Full;
    else if (string == "half") return Neighborhood_Scope::Half;
    return Neighborhood_Scope::Full;
}

bool LocalSearch::GreedyJumper::keep_variable_in_traj(float variable_score, float initial_score) {
    switch (scope)
    {
    case LocalSearch::GreedyJumper::Neighborhood_Scope::Improve: return variable_score > initial_score;
    default: return true;
    }
}
bool LocalSearch::GreedyJumper::do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score) {
    switch (criterion)
    {
    case LocalSearch::GreedyJumper::Selection_Criterion::Least: return (tested_score > init_score) && (first_keep || (tested_score < iteration_best_score));
    case LocalSearch::GreedyJumper::Selection_Criterion::Best: return (tested_score > init_score) && (first_keep || (tested_score > iteration_best_score));
    default: return (tested_score > init_score);
    }
}
bool LocalSearch::GreedyJumper::stop_at_first_improve() {
    switch (criterion)
    {
    case LocalSearch::GreedyJumper::Selection_Criterion::Least: return false;
    case LocalSearch::GreedyJumper::Selection_Criterion::Best: return false;
    default: return true;
    }
}
unsigned int LocalSearch::GreedyJumper::improve(std::unique_ptr<ReversibleInstance>& instance, unsigned int budget, unsigned int initial_budget) {
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    unsigned int better_neighbors_after = better_neighbors;
    output_iteration_ends_data(used_budget, (used_budget  - initial_budget), better_neighbors, score);

    std::set<std::pair<unsigned int, float>, decltype(cmp)> trajectory(cmp);
    while (used_budget < budget)
    {
        // create trajectory
        trajectory.clear();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            if (keep_variable_in_traj(instance->score(), score)) trajectory.insert({i, instance->score()});
            instance->revert_last_mutation();
        }

        // apply all mutations in trajectory
        for (std::pair<unsigned int, float> pair : trajectory) instance->mutate_arg(pair.first);

        // compare jumps of the trajectory
        unsigned int count = trajectory.size();
        unsigned int iteration_best_count = 0;
        float iteration_best_score = score;
        for (auto pair = trajectory.rbegin(); pair != trajectory.rend(); pair++) {
            used_budget++;
            if (do_keep(instance->score(), score, iteration_best_count == 0, iteration_best_score)) {
                iteration_best_score = instance->score();
                iteration_best_count = count;
                if (stop_at_first_improve()) break;
            }
            count--;
            instance->mutate_arg(pair->first);
        }

        // apply best jump found
        float old_score = score;
        score = iteration_best_score;
        if (!stop_at_first_improve()) {
            count = iteration_best_count;
            for (std::pair<unsigned int, float> pair : trajectory) {
                if (count == 0) break;
                count--;
                instance->mutate_arg(pair.first);
            }
        }
        
        better_neighbors_after = count_better_neighbors(instance);
        output_iteration_data(used_budget, (used_budget  - initial_budget), better_neighbors, better_neighbors_after, old_score, score, iteration_best_count);
        better_neighbors = better_neighbors_after;
        if (iteration_best_count == 0) return used_budget;
    }
    return used_budget;
}

LocalSearch::GreedyJumper::GreedyJumper(unsigned int seed, Selection_Criterion criterion, Neighborhood_Scope scope, bool debug): LocalSearch::LocalSearchAlgo(seed, debug), criterion(criterion), scope(scope) {}
LocalSearch::GreedyJumper::GreedyJumper(unsigned int seed, Selection_Criterion criterion, Neighborhood_Scope scope, bool debug, std::ostream* out, bool add_header): LocalSearch::LocalSearchAlgo(seed, debug, out, add_header), criterion(criterion), scope(scope) {}
