#pragma once
#include "../arg_file/parameters.h"
#include "local_search.h"

namespace LocalSearch {
    std::shared_ptr<LocalSearchAlgo> _create_local_search_algo_no_iter(const Parameters& parameters);
    std::shared_ptr<LocalSearchAlgo> create_local_search_algo(const Parameters& parameters);
}