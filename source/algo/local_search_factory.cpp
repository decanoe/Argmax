#include "local_search_factory.h"

#include "iterated_local_search/iterated_local_search.h"

#include "hill_climber/hill_climber.h"
#include "greedy_jumper/greedy_jumper.h"
#include "non_iterated_local_search/tabu/tabu.h"
#include "non_iterated_local_search/one_lambda/one_lambda.h"

std::shared_ptr<LocalSearch::LocalSearchAlgo> LocalSearch::_create_local_search_algo_no_iter(const FileData& file_data) {
    if (file_data.get_string("algorithm") == "hill_climb") {
        return std::make_shared<HillClimber>(
            HillClimber::Selection_Criterion::from_file_data(file_data)
        );
    }
    if (file_data.get_string("algorithm") == "greedy_jumper") {
        return std::make_shared<GreedyJumper>(
            GreedyJumper::Selection_Criterion::from_file_data(file_data),
            GreedyJumper::Neighborhood_Scope::from_file_data(file_data)
        );
    }
    if (file_data.get_string("algorithm") == "tabu_search") {
        return std::make_shared<TabuSearch>(
            file_data.get_float("tabu_size"),
            file_data.get_float("tabu_max_random_size_added"),
            file_data.get_bool("enable_aspiration")
        );
    }
    if (file_data.get_string("algorithm") == "greedy_tabu_search") {
        return std::make_shared<GreedyTabuSearch>(
            file_data.get_float("tabu_size"),
            file_data.get_float("tabu_max_random_size_added"),
            GreedyTabuSearch::push_order_from_string(file_data.get_string("tabu_push_order")),
            file_data.get_bool("enable_aspiration")
        );
    }
    if (file_data.get_string("algorithm") == "one_lambda_search") {
        return std::make_shared<OneLambdaSearch>(
            file_data.get_float("lambda"),
            file_data.get_bool("enable_aspiration")
        );
    }
    if (file_data.get_string("algorithm") == "greedy_one_lambda_search") {
        return std::make_shared<GreedyOneLambdaSearch>(
            file_data.get_float("lambda"),
            file_data.get_bool("enable_aspiration")
        );
    }

    std::cerr << "\033[1;31mThe field <algorithm> of the preset file doesn't correspond to any implemented algorithm.\n\033[0m";
    exit(1);
}
std::shared_ptr<LocalSearch::LocalSearchAlgo> LocalSearch::create_local_search_algo(const FileData& file_data) {
    std::shared_ptr<LocalSearchAlgo> algo = _create_local_search_algo_no_iter(file_data);

    if (file_data.get_bool("iterate", false)) return std::make_shared<IteratedLocalSearch>(algo);
    else return algo;
}
