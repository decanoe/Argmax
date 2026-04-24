#pragma once
#include "../local_search.h"

namespace LocalSearch {
    class IteratedLocalSearch: public LocalSearchAlgo {
    public:
        IteratedLocalSearch(std::shared_ptr<LocalSearchAlgo> algo);

        LocalSearchAlgo* set_debug(bool debug) override;
        LocalSearchAlgo* set_output(std::ostream* out, bool add_header = true) override;

        void improve(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) const override;
    protected:
        std::shared_ptr<LocalSearchAlgo> algo;
    };
}