#include "local_search.h"
#include <limits>

auto cmp = [](std::pair<unsigned int, float> a, std::pair<unsigned int, float> b) {
    if (a.second != b.second) return a.second > b.second;
    else return a.first < b.first;
};

unsigned int count_better_neighbors(std::unique_ptr<ReversibleInstance>& instance) {
    unsigned int result = 0;
    float score = instance->score();
    for (size_t i = 0; i < instance->nb_args(); i++) {
        instance->mutate_arg(i);
        if (instance->score() > score) result++;
        instance->revert_last_mutation();
    }
    return result;
}

LocalSearch::HC_Selection_Criterion LocalSearch::get_HC_Selection_Criterion(const std::string& string) {
    if (string == "random") return HC_Selection_Criterion::Random;
    else if (string == "first") return HC_Selection_Criterion::First;
    else if (string == "cycle") return HC_Selection_Criterion::Cycle;
    else if (string == "least") return HC_Selection_Criterion::Least;
    else if (string == "best") return HC_Selection_Criterion::Best;
    return HC_Selection_Criterion::First;
}
LocalSearch::GJ_Selection_Criterion LocalSearch::get_GJ_Selection_Criterion(const std::string& string) {
    if (string == "first") return GJ_Selection_Criterion::First;
    else if (string == "best") return GJ_Selection_Criterion::Best;
    else if (string == "least") return GJ_Selection_Criterion::Least;
    return GJ_Selection_Criterion::First;
}
LocalSearch::GJ_Neighborhood_Scope LocalSearch::get_GJ_Neighborhood_Scope(const std::string& string) {
    if (string == "improve") return GJ_Neighborhood_Scope::Improve;
    else if (string == "full") return GJ_Neighborhood_Scope::Full;
    else if (string == "half") return GJ_Neighborhood_Scope::Half;
    return GJ_Neighborhood_Scope::Full;
}

unsigned int _get_HC_test_index(LocalSearch::HC_Selection_Criterion criterion, unsigned int default_index, unsigned int iteration_index, const std::set<unsigned int>& visited_indices, unsigned int nb_args) {
    switch (criterion)
    {
    case LocalSearch::HC_Selection_Criterion::Random: return RandomUtils::get_index(nb_args, visited_indices);
    case LocalSearch::HC_Selection_Criterion::Cycle: return (default_index + iteration_index) % nb_args;
    default: return default_index;
    }
}
bool _HC_do_keep(LocalSearch::HC_Selection_Criterion criterion, float tested_score, float init_score, bool first_keep, float iteration_best_score) {
    switch (criterion)
    {
    case LocalSearch::HC_Selection_Criterion::Least: return (tested_score > init_score) && (first_keep || (tested_score < iteration_best_score));
    case LocalSearch::HC_Selection_Criterion::Best: return first_keep || (tested_score > iteration_best_score);
    default: return (tested_score > init_score);
    }
}
bool _HC_stop_at_first_improve(LocalSearch::HC_Selection_Criterion criterion) {
    switch (criterion)
    {
    case LocalSearch::HC_Selection_Criterion::Least: return false;
    case LocalSearch::HC_Selection_Criterion::Best: return false;
    default: return true;
    }
}
unsigned int LocalSearch::hill_climb(std::unique_ptr<ReversibleInstance>& instance, HC_Selection_Criterion criterion, unsigned int budget, unsigned int initial_budget, std::ostream* out, bool add_header) {
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness_before_jump\tnb_better_neighbors_before_jump\tsize_of_the_jump\tfitness_after_jump\tnb_better_neighbors_after_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    unsigned int better_neighbors_after = better_neighbors;
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\t" << score << "\t" << better_neighbors << "\n"; }
    
    unsigned int iteration_count = 0;
    while (used_budget < budget)
    {
        unsigned int iteration_best_move = -1U;
        float iteration_best_score = 0;

        // iterate through mutations
        std::set<unsigned int> visited = std::set<unsigned int>();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            unsigned int index = _get_HC_test_index(criterion, i, iteration_count, visited, instance->nb_args());
            visited.insert(index);

            instance->mutate_arg(index);
            used_budget++;
            if (_HC_do_keep(criterion, instance->score(), score, iteration_best_move == -1U, iteration_best_score)) {
                iteration_best_move = index;
                iteration_best_score = instance->score();
                if (_HC_stop_at_first_improve(criterion))break;
            }
            instance->revert_last_mutation();
        }

        // apply best mutation
        if (iteration_best_move != -1U) {
            float old_score = instance->score();
            instance->mutate_arg(iteration_best_move);
            better_neighbors_after = count_better_neighbors(instance);
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << old_score << "\t" << better_neighbors << "\t1\t" << iteration_best_score << "\t" << better_neighbors_after << "\n"; }
            better_neighbors = better_neighbors_after;
            score = iteration_best_score;
        }
        else {
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\t" << score << "\t" << better_neighbors << "\n"; }
            break;
        }
        iteration_count++;
    }

    return used_budget;
}


bool _GJ_keep_variable_in_traj(LocalSearch::GJ_Neighborhood_Scope scope, float variable_score, float initial_score) {
    switch (scope)
    {
    case LocalSearch::GJ_Neighborhood_Scope::Improve: return variable_score > initial_score;
    default: return true;
    }
}
bool _GJ_do_keep(LocalSearch::GJ_Selection_Criterion criterion, float tested_score, float init_score, bool first_keep, float iteration_best_score) {
    switch (criterion)
    {
    case LocalSearch::GJ_Selection_Criterion::Least: return (tested_score > init_score) && (first_keep || (tested_score < iteration_best_score));
    case LocalSearch::GJ_Selection_Criterion::Best: return first_keep || (tested_score > iteration_best_score);
    default: return (tested_score > init_score);
    }
}
bool _GJ_stop_at_first_improve(LocalSearch::GJ_Selection_Criterion criterion) {
    switch (criterion)
    {
    case LocalSearch::GJ_Selection_Criterion::Least: return false;
    case LocalSearch::GJ_Selection_Criterion::Best: return false;
    default: return true;
    }
}
unsigned int LocalSearch::greedy_jumper(std::unique_ptr<ReversibleInstance>& instance, GJ_Selection_Criterion criterion, GJ_Neighborhood_Scope scope, unsigned int budget, unsigned int initial_budget, std::ostream* out, bool add_header) {
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness_before_jump\tnb_better_neighbors_before_jump\tsize_of_the_jump\tfitness_after_jump\tnb_better_neighbors_after_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    unsigned int better_neighbors_after = better_neighbors;
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\t" << score << "\t" << better_neighbors << "\n"; }

    std::set<std::pair<unsigned int, float>, decltype(cmp)> trajectory(cmp);
    while (used_budget < budget)
    {
        // create trajectory
        trajectory.clear();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            if (_GJ_keep_variable_in_traj(scope, instance->score(), score)) trajectory.insert({i, instance->score()});
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
            if (_GJ_do_keep(criterion, instance->score(), score, iteration_best_count == 0, iteration_best_score)) {
                iteration_best_score = instance->score();
                iteration_best_count = count;
                if (_GJ_stop_at_first_improve(criterion)) break;
            }
            count--;
            instance->mutate_arg(pair->first);
        }

        // apply best jump found
        float old_score = score;
        score = iteration_best_score;
        if (!_GJ_stop_at_first_improve(criterion)) {
            count = iteration_best_count;
            for (std::pair<unsigned int, float> pair : trajectory) {
                if (count == 0) break;
                count--;
                instance->mutate_arg(pair.first);
            }
        }
        
        better_neighbors_after = count_better_neighbors(instance);
        if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << old_score << "\t" << better_neighbors << "\t" << iteration_best_count << "\t" << score << "\t" << better_neighbors_after << "\n"; }
        better_neighbors = better_neighbors_after;
        if (iteration_best_count == 0) return used_budget;
    }
    return used_budget;
}
