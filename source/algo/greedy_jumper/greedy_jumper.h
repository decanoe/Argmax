#pragma once
#include "../local_search.h"

namespace LocalSearch {
    class GreedyJumper: public LocalSearchAlgo {
    public:
        typedef std::set<std::pair<unsigned int, float>, std::function<bool(std::pair<unsigned int, float>, std::pair<unsigned int, float>)>> TrajectorySet;

        class Selection_Criterion: public LocalSearchAlgoComponent {
        public:
            Selection_Criterion() = default;
            Selection_Criterion(const Selection_Criterion&) = delete;

            static std::shared_ptr<Selection_Criterion> from_file_data(const FileData& file_data);

            /// @brief applies the best multi jump from the trajectory depending on the criterion and returns the number of bits flipped
            /// @param trajectory the trajectory set
            /// @param instance the instance on which to do the jump
            /// @param instance_score the score of the instance (will be set to the new score before returning)
            /// @param current_budget the current budget to update
            /// @param budget the budget to update
            /// @return the number of bit flipped (or 0 if no jump was done)
            virtual unsigned int chose_jump(const TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float& instance_score, BudgetHelper& budget) const;
        protected:
            /// @brief whether the provided tested score is better than the current one (in regard to the strategy)
            /// @param tested_score the newly tested score
            /// @param init_score the score at the begining of the iteration
            /// @param first_test true if there is no bette score yet
            /// @param iteration_best_score the best score for this iteration (yet)
            /// @return true if the provided score needs to be kept
            virtual bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score) const = 0;
            /// @brief whether the algorithm needs to stop at the first improving score
            /// @return true if the algorithm needs to stop at the first improving score
            virtual bool stop_at_first_improve() const = 0;
        };
        class Neighborhood_Scope: public LocalSearchAlgoComponent {
        public:
            Neighborhood_Scope() = default;
            Neighborhood_Scope(const Neighborhood_Scope&) = delete;

            static std::shared_ptr<Neighborhood_Scope> from_file_data(const FileData& file_data);

            /// @brief fill the TrajectorySet with all valid variable flips
            /// @param trajectory the TrajectorySet to fill
            /// @param instance the instance from which to create the TrajectorySet
            /// @param score the score of the instance (to avoid unnecessary budget consumption)
            /// @param budget the budget to update
            void create_trajectory(TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget) const;
            /// @brief fill the TrajectorySet with all valid variable flips
            /// @param trajectory the TrajectorySet to fill
            /// @param instance the instance from which to create the TrajectorySet
            /// @param score the score of the instance (to avoid unnecessary budget consumption)
            /// @param budget the budget to update
            /// @param is_valid a function returning weather a specific bit can be added to the trajectory (used by tabu)
            virtual void create_trajectory(TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget, std::function<bool(unsigned int)> is_valid) const = 0;
        protected:
        };

        /// @brief creates a GreedyJumper algo structure
        /// @param criterion the move selection criterion
        /// @param scope the scope of the variables used for agregating jumps
        GreedyJumper(std::shared_ptr<Selection_Criterion> criterion, std::shared_ptr<Neighborhood_Scope> scope);

        LocalSearchAlgo* set_seed(std::shared_ptr<std::mt19937> random_generator) override;

        static bool cmp(std::pair<unsigned int, float>, std::pair<unsigned int, float>);
    protected:
        std::shared_ptr<Selection_Criterion> criterion;
        std::shared_ptr<Neighborhood_Scope> scope;
        
        bool improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget) override;
    };
}