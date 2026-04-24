#include "iterated_local_search.h"

using namespace LocalSearch;

IteratedLocalSearch::IteratedLocalSearch(std::shared_ptr<LocalSearchAlgo> algo): LocalSearchAlgo(), algo(algo) {}

LocalSearchAlgo* IteratedLocalSearch::set_debug(bool debug) {
    algo->set_debug(debug);
    return LocalSearchAlgo::set_debug(debug);
}
LocalSearchAlgo* IteratedLocalSearch::set_output(std::ostream* out, bool add_header) {
    algo->set_output(out, add_header);
    return LocalSearchAlgo::set_output(out, false);
}

void IteratedLocalSearch::improve(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) const {
    std::mt19937 copied_random_generator = *random_generator;

    while (!budget.out_of_budget())
    {
        std::unique_ptr<ReversibleInstance> temp = instance->randomize_clone(*random_generator);

        algo->set_seed(std::shared_ptr<std::mt19937>(new std::mt19937(copied_random_generator)));
        copied_random_generator(); // step once to make sure the generator is different for each iterated algo call

        budget.new_run();
        algo->improve(temp, budget);

        if (temp->score() > instance->score()) instance = std::move(temp);
    }
}
