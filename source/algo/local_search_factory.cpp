#include "local_search_factory.h"

#include "iterated_local_search/iterated_local_search.h"

#include "hill_climber/hill_climber.h"
#include "greedy_jumper/greedy_jumper.h"

std::shared_ptr<LocalSearch::LocalSearchAlgo> LocalSearch::_create_local_search_algo_no_iter(const FileData& file_data) {
    if (file_data.get_string("algorithm") == "hill_climb") {
        return std::make_shared<HillClimber>(
            HillClimber::Selection_Criterion::from_file_data(file_data)
        );
    }
    else if (file_data.get_string("algorithm") == "greedy_jumper") {
        return std::make_shared<GreedyJumper>(
            GreedyJumper::Selection_Criterion::from_file_data(file_data),
            GreedyJumper::Neighborhood_Scope::from_file_data(file_data)
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
