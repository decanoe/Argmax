#pragma once
#include "../arg_file/file_data.h"
#include "local_search.h"

namespace LocalSearch {
    std::shared_ptr<LocalSearchAlgo> _create_local_search_algo_no_iter(const FileData& file_data);
    std::shared_ptr<LocalSearchAlgo> create_local_search_algo(const FileData& file_data);
}