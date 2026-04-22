#pragma once
#include "../arg_file/file_data.h"
#include "../instance/reversible_instance.h"
#include "../utils/random_utils.h"
#include <iostream>
#include <fstream>
#include <functional>
#include <random>

namespace LocalSearch {
    class LocalSearchAlgo {
    public:
        LocalSearchAlgo();
        LocalSearchAlgo(const LocalSearchAlgo&) = delete;

        /// @brief sets the random generator engine
        /// @param random_generator the random engine to use
        /// @return itself
        virtual LocalSearchAlgo* set_seed(std::shared_ptr<std::mt19937> random_generator);
        /// @brief sets whether the algo should debug infos on the std::cout stream
        /// @param debug should the algo debug infos on std::cout
        /// @return itself
        virtual LocalSearchAlgo* set_debug(bool debug);
        /// @brief sets the stream output of the algo (for run data saving)
        /// @param out a stream on wich to forward results (or nullprt if not wanted)
        /// @param add_header a boolean set to true if the header need to be added to the output stream
        /// @return itself
        virtual LocalSearchAlgo* set_output(std::ostream* out, bool add_header = true);

        /// @brief runs the local search algo on a specific instance with a given budget
        /// @param instance the instance to improve
        /// @param budget the budget limit
        /// @param initial_budget the budget already used at the start of the run (usefull for iterated runs)
        /// @return the budget at the end of the improvement
        virtual unsigned int improve(std::unique_ptr<ReversibleInstance>& instance, unsigned int budget = 1024, unsigned int initial_budget = 0) const = 0;
    protected:
        std::shared_ptr<std::mt19937> random_generator;
        // the output stream
        std::ostream* out;
        // should the algo debug infos on std::cout
        bool debug;

        /// @brief adds a line to the output stream
        /// @param used_budget the budget used since the start of all the iterated runs
        /// @param in_run_budget the budget used since the start of this run
        /// @param nb_better_neighbors_before_jump the number of improving neighbors before the jump
        /// @param nb_better_neighbors_after_jump the number of improving neighbors after the jump
        /// @param old_score the score before the jump
        /// @param new_score the score after the jump
        /// @param jump_size the number of bit flipped during the jump
        void output_iteration_data(unsigned int used_budget, unsigned int in_run_budget, unsigned int nb_better_neighbors_before_jump, unsigned int nb_better_neighbors_after_jump, float old_score, float new_score, unsigned int jump_size) const;
        /// @brief adds a line to the output stream representing the start or the end of a run
        /// @param used_budget the budget used since the start of all the iterated runs
        /// @param in_run_budget the budget used since the start of this run
        /// @param nb_better_neighbors the number of improving neighbors
        /// @param score the score
        void output_iteration_ends_data(unsigned int used_budget, unsigned int in_run_budget, unsigned int nb_better_neighbors, float score) const;
        
        /// @brief counts the number of improving neighbors of a instance
        /// @param instance the instance we want the number of improving neighbors of
        /// @return the number of improving neighbors of the instance
        unsigned int count_better_neighbors(std::unique_ptr<ReversibleInstance>& instance) const;
    };
    
    class LocalSearchAlgoComponent {
    public:
        LocalSearchAlgoComponent() = default;
        LocalSearchAlgoComponent(const LocalSearchAlgoComponent&) = delete;
        
        /// @brief sets the random generator engine
        /// @param random_generator the random engine to use
        /// @return itself
        LocalSearchAlgoComponent* set_seed(std::shared_ptr<std::mt19937> random_generator);
    protected:
        std::shared_ptr<std::mt19937> random_generator;
    };
}