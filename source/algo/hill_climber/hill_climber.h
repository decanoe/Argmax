#pragma once
#include "../local_search.h"

namespace LocalSearch {
    class HillClimber: public LocalSearchAlgo {
    public:
        class Selection_Criterion: public LocalSearchAlgoComponent {
        public:
            Selection_Criterion() = default;

            static std::shared_ptr<Selection_Criterion> from_file_data(const Parameters& parameters);
            
            /// @brief gets the variable index to test depending on the strategy
            /// @param default_index the default index
            /// @param iteration_index the iteration index in the run
            /// @param visited_indices a set of all visited indices so far
            /// @param nb_args the number of variables of the instance
            /// @return the index to test (or nb_args if no index is left)
            virtual unsigned int get_test_index(unsigned int default_index, unsigned int iteration_index, unsigned int nb_args);
            /// @brief whether the provided tested score is better than the current one (in regard to the strategy)
            /// @param tested_score the newly tested score
            /// @param init_score the score at the begining of the iteration
            /// @param first_test true if there is no bette score yet
            /// @param iteration_best_score the best score for this iteration (yet)
            /// @return true if the provided score needs to be kept
            virtual bool do_keep(float tested_score, float init_score, bool first_test, float iteration_best_score) const = 0;
            /// @brief whether the algorithm needs to stop at the first improving score
            /// @return true if the algorithm needs to stop at the first improving score
            virtual bool stop_at_first_improve() const = 0;
        protected:
        };

        /// @brief creates a HillClimber algo structure
        /// @param criterion the move selection criterion
        HillClimber(std::shared_ptr<Selection_Criterion> criterion);

        LocalSearchAlgo* set_seed(std::shared_ptr<std::mt19937> random_generator) override;
        
        float run(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) override;
    protected:
        std::shared_ptr<Selection_Criterion> criterion;
        unsigned int iteration_count;

        bool improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget);
    };
}