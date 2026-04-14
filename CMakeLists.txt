cmake_minimum_required(VERSION 3.10.0)
project(Argmax VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror)

add_executable(${CMAKE_PROJECT_NAME}
    source/main.cpp

    source/algo/argmax.cpp
    source/algo/local_search.cpp
    
    source/arg_file/file_data.cpp

    source/utils/random_utils.cpp

    source/problem/nk.cpp

    source/instance/reversible_instance.cpp
    source/instance/fitness_instance.cpp
)
