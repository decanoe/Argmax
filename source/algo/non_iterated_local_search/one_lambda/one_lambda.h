#pragma once
#include "../non_iterated_local_search.h"

namespace LocalSearch
{
    class OneLambdaSearch: public NonIteratedLocalSearch {
    public:
        /// @brief creates a TabuSearch algo structure
        /// @param lambda number of neighbors to check in proportion with the dimension
        OneLambdaSearch(float lambda, bool enable_aspiration);
    protected:
        float lambda;

        bool improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget, float aspiration_score) override;
    };

    class GreedyOneLambdaSearch: public OneLambdaSearch {
    public:
        /// @brief creates a GreedyTabuSearch algo structure
        /// @param lambda number of neighbors to check in proportion with the dimension
        GreedyOneLambdaSearch(float lambda, bool enable_aspiration);
    protected:
        bool improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget, float aspiration_score) override;
    };
}