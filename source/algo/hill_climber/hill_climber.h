#pragma once
#include "../local_search.h"

namespace LocalSearch {
    class HillClimber: public LocalSearchAlgo {
    public:
        enum class Selection_Criterion { Random, First, Cycle, Best, Least };
        static Selection_Criterion get_Selection_Criterion(const std::string& string);

        /// @brief creates a HillClimber algo structure
        /// @param seed the seed for the random engine
        /// @param criterion the move selection criterion
        /// @param debug should the algo debug infos on std::cout
        HillClimber(unsigned int seed, Selection_Criterion criterion, bool debug);
        /// @brief creates a HillClimber algo structure with a stream output
        /// @param seed the seed for the random engine
        /// @param criterion the move selection criterion
        /// @param debug should the algo debug infos on std::cout
        /// @param out a stream on wich to forward results (or nullprt if not wanted)
        /// @param add_header a boolean set to true if the header need to be added to the output stream
        HillClimber(unsigned int seed, Selection_Criterion criterion, bool debug, std::ostream* out, bool add_header = true);

        unsigned int improve(std::unique_ptr<ReversibleInstance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0) override;
    protected:
        Selection_Criterion criterion;

        /// @brief gets the variable index to test depending on the strategy
        /// @param default_index the default index
        /// @param iteration_index the iteration index in the run
        /// @param visited_indices a set of all visited indices so far
        /// @param nb_args the number of variables of the instance
        /// @return the index to test (or -1 if no index is left)
        unsigned int get_test_index(unsigned int default_index, unsigned int iteration_index, const std::set<unsigned int>& visited_indices, unsigned int nb_args);
        /// @brief whether the provided tested score is better than the current one (in regard to the strategy)
        /// @param tested_score the newly tested score
        /// @param init_score the score at the begining of the iteration
        /// @param first_test true if there is no bette score yet
        /// @param iteration_best_score the best score for this iteration (yet)
        /// @return true if the provided score needs to be kept
        bool do_keep(float tested_score, float init_score, bool first_test, float iteration_best_score);
        /// @brief whether the algorithm needs to stop at the first improving score
        /// @return true if the algorithm needs to stop at the first improving score
        bool stop_at_first_improve();
    };
}