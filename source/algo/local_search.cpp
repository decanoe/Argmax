#include "local_search.h"

LocalSearch::LocalSearchAlgo::LocalSearchAlgo(unsigned int seed, bool debug): random_generator(seed), out(nullptr), debug(debug) {}
LocalSearch::LocalSearchAlgo::LocalSearchAlgo(unsigned int seed, bool debug, std::ostream* out, bool add_header): random_generator(seed), out(out), debug(debug) {
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness_before_jump\tnb_better_neighbors_before_jump\tsize_of_the_jump\tfitness_after_jump\tnb_better_neighbors_after_jump" << "\n"; }
}
void LocalSearch::LocalSearchAlgo::set_seed(unsigned int seed) {
    this->random_generator = std::mt19937(seed);
}

unsigned int LocalSearch::LocalSearchAlgo::count_better_neighbors(std::unique_ptr<ReversibleInstance>& instance) const {
    unsigned int result = 0;
    float score = instance->score();
    for (size_t i = 0; i < instance->nb_args(); i++) {
        instance->mutate_arg(i);
        if (instance->score() > score) result++;
        instance->revert_last_mutation();
    }
    return result;
}

void LocalSearch::LocalSearchAlgo::output_iteration_data(unsigned int used_budget, unsigned int in_run_budget, unsigned int nb_better_neighbors_before_jump, unsigned int nb_better_neighbors_after_jump, float old_score, float new_score, unsigned int jump_size) const {
    if (out) { *out << used_budget << "\t" << in_run_budget << "\t" << old_score << "\t" << nb_better_neighbors_before_jump << "\t" << jump_size << "\t" << new_score << "\t" << nb_better_neighbors_after_jump << "\n"; }
}
void LocalSearch::LocalSearchAlgo::output_iteration_ends_data(unsigned int used_budget, unsigned int in_run_budget, unsigned int nb_better_neighbors, float score) const {
    if (out) { *out << used_budget << "\t" << in_run_budget << "\t" << score << "\t" << nb_better_neighbors << "\t" << 0 << "\t" << score << "\t" << nb_better_neighbors << "\n"; }
}