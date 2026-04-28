#pragma once
#include "../local_search.h"

namespace LocalSearch {
    class IteratedLocalSearch: public LocalSearchAlgo {
    public:
        IteratedLocalSearch(std::shared_ptr<LocalSearchAlgo> algo);

        LocalSearchAlgo* set_debug(bool debug) override;
        LocalSearchAlgo* set_output(std::ostream* out, bool add_header = true) override;

        float run(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) override;
    protected:
        std::shared_ptr<LocalSearchAlgo> algo;

        bool improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget) override;
    };
}