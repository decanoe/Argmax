#pragma once
#include "../hill_climber/hill_climber.h"
#include "../greedy_jumper/greedy_jumper.h"
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

    class TabuSearch: public HillClimber {
    public:
        /// @brief creates a TabuSearch algo structure
        /// @param criterion the move selection criterion
        TabuSearch(std::shared_ptr<Selection_Criterion> criterion, float tabu_size, float max_random_size_added);

        void improve(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) const override;
    protected:
        float tabu_size;
        float tabu_max_random_size_added;
    };

    class GreedyTabuSearch: public GreedyJumper {
    public:
        enum class TabuPushOrder { BestToWorst, BestToWorstClamped, WorstToBest };
        static TabuPushOrder push_order_from_string(const std::string& string);
        
        /// @brief creates a GreedyTabuSearch algo structure
        /// @param criterion the move selection criterion
        /// @param scope the scope of the variables used for agregating jumps
        GreedyTabuSearch(std::shared_ptr<Selection_Criterion> criterion, std::shared_ptr<Neighborhood_Scope> scope, float tabu_size, float max_random_size_added, TabuPushOrder tabu_push_order);
        
        void improve(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) const override;
    protected:
        TabuPushOrder tabu_push_order;
        float tabu_size;
        float tabu_max_random_size_added;
    };

}}