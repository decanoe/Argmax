#pragma once
#include "../local_search.h"

namespace LocalSearch {
    class GreedyJumper: public LocalSearchAlgo {
    public:
        enum class Selection_Criterion { First, Best, Least };
        enum class Neighborhood_Scope { Full, Improve, Half };
        static Selection_Criterion get_Selection_Criterion(const std::string& string);
        static Neighborhood_Scope get_Neighborhood_Scope(const std::string& string);

        /// @brief creates a HillClimber algo structure
        /// @param seed the seed for the random engine
        /// @param criterion the move selection criterion
        /// @param scope the scope of the variables used for agregating jumps
        /// @param debug should the algo debug infos on std::cout
        GreedyJumper(unsigned int seed, Selection_Criterion criterion, Neighborhood_Scope scope, bool debug);
        /// @brief creates a HillClimber algo structure with a stream output
        /// @param seed the seed for the random engine
        /// @param criterion the move selection criterion
        /// @param scope the scope of the variables used for agregating jumps
        /// @param debug should the algo debug infos on std::cout
        /// @param out a stream on wich to forward results (or nullprt if not wanted)
        /// @param add_header a boolean set to true if the header need to be added to the output stream
        GreedyJumper(unsigned int seed, Selection_Criterion criterion, Neighborhood_Scope scope, bool debug, std::ostream* out, bool add_header = true);

        unsigned int improve(std::unique_ptr<ReversibleInstance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0) override;
    protected:
        Selection_Criterion criterion;
        Neighborhood_Scope scope;

        /// @brief whether the variable should be kept in the trajectory based on it's score
        /// @param variable_score the score if we flip the variable
        /// @param init_score the score at the begining of the iteration
        /// @return true if the variable should be kept in the trajectory
        bool keep_variable_in_traj(float variable_score, float initial_score);
        /// @brief whether the provided tested score is better than the current one (in regard to the strategy)
        /// @param tested_score the newly tested score
        /// @param init_score the score at the begining of the iteration
        /// @param first_test true if there is no bette score yet
        /// @param iteration_best_score the best score for this iteration (yet)
        /// @return true if the provided score needs to be kept
        bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score);
        /// @brief whether the algorithm needs to stop at the first improving score
        /// @return true if the algorithm needs to stop at the first improving score
        bool stop_at_first_improve();
    };
}