#pragma once
#include "../local_search.h"

namespace LocalSearch {
    class GreedyJumper: public LocalSearchAlgo {
    public:
        typedef std::set<std::pair<unsigned int, float>, std::function<bool(std::pair<unsigned int, float> a, std::pair<unsigned int, float> b)>> TrajectorySet;

        class Selection_Criterion {
        public:
            Selection_Criterion() = default;
            Selection_Criterion(const Selection_Criterion&) = delete;

            static std::shared_ptr<Selection_Criterion> from_file_data(const FileData& file_data);
        
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
        protected:
        };
        class Neighborhood_Scope {
        public:
            Neighborhood_Scope() = default;
            Neighborhood_Scope(const Neighborhood_Scope&) = delete;

            static std::shared_ptr<Neighborhood_Scope> from_file_data(const FileData& file_data);
            
            /// @brief reset the scope variables to prepare for a new run (usefull for tabu search for example)
            virtual void reset() = 0;

            /// @brief fill the TrajectorySet with all valid variable flips
            /// @param trajectory the TrajectorySet to fill
            /// @param instance the instance from which to create the TrajectorySet
            /// @param score the score of the instance (to avoid unnecessary budget consumption)
            /// @return the budget consomed by the trajectory creation
            virtual unsigned int create_trajectory(TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score) = 0;
        protected:
        };

        /// @brief creates a GreedyJumper algo structure
        /// @param criterion the move selection criterion
        /// @param scope the scope of the variables used for agregating jumps
        GreedyJumper(std::shared_ptr<Selection_Criterion> criterion, std::shared_ptr<Neighborhood_Scope> scope);

        unsigned int improve(std::unique_ptr<ReversibleInstance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0) const override;
    protected:
        std::shared_ptr<Selection_Criterion> criterion;
        std::shared_ptr<Neighborhood_Scope> scope;
    };


}