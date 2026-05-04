#include "problem_factory.h"
#include "nk.h"
#include "qubo.h"
#include "sat.h"

std::shared_ptr<Problem::Problem> Problem::create_problem(const Parameters& parameters, unsigned int seed, bool debug) {
    if (parameters.get_string("problem") == "NK") {
        if (parameters.contains_string("instance_path")) {
            if (debug) std::cout << "reading file..." << std::endl;
            return std::make_shared<NK>(parameters.get_string("instance_path"));
        }
        else {
            if (debug) std::cout << "creating problem..." << std::endl;
            std::mt19937 rand(seed);
            return std::make_shared<NK>(parameters.get_int("N"), parameters.get_int("K"), rand);
        }
    }

    if (parameters.get_string("problem") == "Qubo") {
        if (parameters.contains_string("instance_path")) {
            if (debug) std::cout << "reading file..." << std::endl;
            return std::make_shared<Qubo>(parameters.get_string("instance_path"));
        }
        else {
            if (debug) std::cout << "creating problem..." << std::endl;
            std::mt19937 rand(seed);
            return std::make_shared<Qubo>(parameters.get_int("N"), rand);
        }
    }

    if (parameters.get_string("problem") == "Sat") {
        if (parameters.contains_string("instance_path")) {
            if (debug) std::cout << "reading file..." << std::endl;
            return std::make_shared<Sat>(parameters.get_string("instance_path"));
        }
    }
    
    std::cerr << "\033[1;31mThe field <problem> of the preset file doesn't correspond to any implemented algorithm.\n\033[0m";
    exit(1);
}
