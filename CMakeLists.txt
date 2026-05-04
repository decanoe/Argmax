cmake_minimum_required(VERSION 3.10.0)
project(Argmax VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror)

add_executable(${CMAKE_PROJECT_NAME}
    source/main.cpp

    source/algo/local_search.cpp
    source/algo/local_search_factory.cpp
    source/algo/hill_climber/hill_climber.cpp
    source/algo/greedy_jumper/greedy_jumper.cpp
    source/algo/iterated_local_search/iterated_local_search.cpp
    source/algo/non_iterated_local_search/non_iterated_local_search.cpp
    source/algo/non_iterated_local_search/tabu/tabu.cpp
    source/algo/non_iterated_local_search/one_lambda/one_lambda.cpp
    
    source/arg_file/job.cpp
    source/arg_file/job_queue.cpp
    source/arg_file/parameters.cpp
    source/arg_file/run_parameters.cpp

    source/utils/random_utils.cpp
    source/utils/budget_helper.cpp

    source/problem/nk.cpp
    source/problem/qubo.cpp
    source/problem/sat.cpp
    source/problem/problem_factory.cpp

    source/instance/reversible_instance.cpp
    source/instance/fitness_instance.cpp
)
