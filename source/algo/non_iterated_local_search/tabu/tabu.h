#pragma once
#include "../non_iterated_local_search.h"
#include "../../greedy_jumper/greedy_jumper.h"
#include <deque>

namespace LocalSearch { namespace Tabu
{
    class TabuList {
    public:
        TabuList(size_t size);
        TabuList(size_t default_size, size_t max_random_size_added, std::shared_ptr<std::mt19937> random_generator);

        void randomize_size();
        void push(unsigned int value);
        bool contains(unsigned int value) const;

        size_t max_size() const;
    protected:
        size_t default_size;
        size_t current_size;
        size_t random_max;
        std::deque<unsigned int> tabu_queue;
        std::shared_ptr<std::mt19937> random_generator;
    };

    class TabuSearch: public NonIteratedLocalSearch {
    public:
        /// @brief creates a TabuSearch algo structure
        /// @param tabu_size number of flips rememberd in the tabu list in proportion with the dimension
        /// @param max_random_size_added maximum number added to the base tabu_size each iteration in proportion with the dimension
        TabuSearch(float tabu_size, float max_random_size_added, bool enable_aspiration);

        float run(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) override;
    protected:
        float tabu_size;
        float tabu_max_random_size_added;
        TabuList tabu_list;

        bool improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget, float aspiration_score) override;
    };

    class GreedyTabuSearch: public TabuSearch {
    public:
        enum class TabuPushOrder { BestToWorst, BestToWorstClamped, WorstToBest, Random };
        static TabuPushOrder push_order_from_string(const std::string& string);
        
        /// @brief creates a GreedyTabuSearch algo structure
        /// @param tabu_size number of flips rememberd in the tabu list in proportion with the dimension
        /// @param max_random_size_added maximum number added to the base tabu_size each iteration in proportion with the dimension
        GreedyTabuSearch(float tabu_size, float max_random_size_added, TabuPushOrder tabu_push_order, bool enable_aspiration);
    protected:
        TabuPushOrder tabu_push_order;

        void tabu_push(const GreedyJumper::TrajectorySet& trajectory, unsigned int jump_size);
        bool improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget, float aspiration_score) override;
    };
}}