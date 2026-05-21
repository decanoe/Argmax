#include "local_search_factory.h"

#include "iterated_local_search/iterated_local_search.h"

#include "hill_climber/hill_climber.h"
#include "greedy_jumper/greedy_jumper.h"
#include "non_iterated_local_search/tabu/tabu.h"
#include "non_iterated_local_search/one_lambda/one_lambda.h"

std::shared_ptr<LocalSearch::LocalSearchAlgo> LocalSearch::_create_local_search_algo_no_iter(const Parameters& parameters) {
    std::string algo = parameters.get_string("algorithm");

    if (algo == "hill_climb") {
        return std::make_shared<HillClimber>(
            HillClimber::Selection_Criterion::from_file_data(parameters)
        );
    }
    if (algo == "greedy_jumper") {
        return std::make_shared<GreedyJumper>(
            GreedyJumper::Selection_Criterion::from_file_data(parameters),
            GreedyJumper::Neighborhood_Scope::from_file_data(parameters),
            GreedyTrajectory::ordering_from_string(parameters.get_string("positive_ordering", "Desc")),
            GreedyTrajectory::ordering_from_string(parameters.get_string("negative_ordering", "Desc"))
        );
    }
    if (algo == "tabu_search") {
        return std::make_shared<TabuSearch>(
            parameters.get_float("tabu_size"),
            parameters.get_float("tabu_max_random_size_added"),
            parameters.get_bool("enable_aspiration")
        );
    }
    if (algo == "greedy_tabu_search") {
        return std::make_shared<GreedyTabuSearch>(
            parameters.get_float("tabu_size"),
            parameters.get_float("tabu_max_random_size_added"),
            GreedyTabuSearch::push_order_from_string(parameters.get_string("tabu_push_order")),
            parameters.get_bool("enable_aspiration"),
            GreedyTrajectory::ordering_from_string(parameters.get_string("positive_ordering", "Desc")),
            GreedyTrajectory::ordering_from_string(parameters.get_string("negative_ordering", "Desc"))
        );
    }
    if (algo == "one_lambda_search") {
        return std::make_shared<OneLambdaSearch>(
            parameters.get_float("lambda"),
            parameters.get_bool("enable_aspiration")
        );
    }
    if (algo == "greedy_one_lambda_search") {
        return std::make_shared<GreedyOneLambdaSearch>(
            parameters.get_float("lambda"),
            parameters.get_bool("enable_aspiration"),
            GreedyTrajectory::ordering_from_string(parameters.get_string("positive_ordering", "Desc")),
            GreedyTrajectory::ordering_from_string(parameters.get_string("negative_ordering", "Desc"))
        );
    }

    std::cerr << "\033[1;31mThe field <algorithm> \"" << algo << "\" of the preset file doesn't correspond to any implemented algorithm.\n\033[0m";
    exit(1);
}
std::shared_ptr<LocalSearch::LocalSearchAlgo> LocalSearch::create_local_search_algo(const Parameters& parameters) {
    std::shared_ptr<LocalSearchAlgo> algo = _create_local_search_algo_no_iter(parameters);

    if (parameters.get_bool("iterate", false)) return std::make_shared<IteratedLocalSearch>(algo);
    else return algo;
}
