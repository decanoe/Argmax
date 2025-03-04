cmake_minimum_required(VERSION 3.5.0)
project(Argmax VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
    source/argmax.cpp

    sat_specific/clause.cpp
    sat_specific/formule.cpp
    sat_specific/solution.cpp
)
