#include <random>

#include "arg_file/run_parameters.h"
#include "arg_file/job_queue.h"
#include "problem/nk.h"

void path_message() {
    std::cerr << "\033[1;31mIf you want to run an algorithm you need to put a path to a file with all the informations\n\033[0m";
    std::cerr << "\033[1;31mIf you want to launch the visualizer please use -visualize or -v\n\033[0m";
    std::cerr << "\033[1;31mIf you want to test a Faraway hand please use -test or -t followed by all the card indices\n\033[0m";
    std::cerr << "\033[1;31mIf you want to create a NK instance please use -create_nk or -nk followed by the N and the K wanted and a path to save the instance\n\033[0m";
    exit(1);
}

void create_all_NK_instances() {
    std::random_device rd;
    std::mt19937 rand(rd());
    for (unsigned int N : { 50, 100, 200 })
    for (unsigned int K : { 0, 1, 2, 4, 8 })
    for (unsigned int I = 0; I < 10; I++)
    {
        Problem::NK nk(N, K, rand);
        nk.save_to_file("./NK/instances/" + std::to_string(N) + "_" + std::to_string(K) + "_" + std::to_string(I) + ".nk");
    }
}

int main(int argc, char *args[]) {
    if (argc < 2) path_message();

    std::string arg1 = args[1];
    if (arg1 == "-visualize" || arg1 == "-v") {
        system("python ./python/data_visualizer.py");
        return 0;
    }
    else if (arg1 == "-create_nk" || arg1 == "-nk") {
        if (argc < 5) {
            std::cerr << "\033[1;31mIf you want to create an nk instance you need to put a N, a K and a path to save the instance.\n\033[0m";
            exit(1);
        }
        
        std::random_device rd;
        std::mt19937 rand(rd());
        Problem::NK nk(atoi(args[2]), atoi(args[3]), rand);
        nk.save_to_file(args[4]);
        return 0;
    }

    RunParameters run_parameters = RunParameters(arg1);

    std::random_device rd;
    auto seed = rd();
    run_parameters.set_default_seed(seed);

    JobQueue jobs = JobQueue(run_parameters);

    if (argc > 2 && std::string(args[2]) == "-t" && jobs.get_job_count() != 1) {
        jobs.launch();
        jobs.join();
    }
    else jobs.launch(0);
}