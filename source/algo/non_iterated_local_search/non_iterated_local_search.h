#pragma once
#include "../local_search.h"

namespace LocalSearch {
    class NonIteratedLocalSearch: public LocalSearchAlgo {
    public:
        NonIteratedLocalSearch(bool enable_aspiration);

        float run(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) override;
    protected:
        bool aspiration;

        bool improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget) override final;
        virtual bool improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget, float aspiration_score) = 0;
    };
}