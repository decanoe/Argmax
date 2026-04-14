#include <ctime>
#include <chrono>
#include <filesystem>
#include <regex>
#include <thread>
#include <mutex>
#include <sstream>
#include <random>

#include "algo/argmax.h"
#include "algo/local_search.h"
#include "arg_file/file_data.h"
#include "instance/fitness_instance.h"
#include "problem/nk.h"

std::string timestamp() {
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof (buffer), "%Hh%M_%d_%m_%Y", timeinfo);
    return std::string (buffer);
}

void path_message() {
    std::cerr << "\033[1;31mIf you want to run an algorithm you need to put a path to a file with all the informations\n\033[0m";
    std::cerr << "\033[1;31mIf you want to launch the visualizer please use -visualize or -v\n\033[0m";
    std::cerr << "\033[1;31mIf you want to test a Faraway hand please use -test or -t followed by all the card indices\n\033[0m";
    std::cerr << "\033[1;31mIf you want to create a NK instance please use -create_nk or -nk followed by the N and the K wanted and a path to save the instance\n\033[0m";
    exit(1);
}

unsigned int run(const FileData& file_data, std::unique_ptr<ReversibleInstance>& instance, std::mt19937& rand, std::ostream* output_file = nullptr, unsigned int used_budget = 0) {
    if (file_data.get_string("algorithm") == "hill_climb")
        return LocalSearch::hill_climb(instance, LocalSearch::get_HC_Selection_Criterion(file_data.get_string("selection_criterion")), rand, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
    else if (file_data.get_string("algorithm") == "greedy_jumper")
        return LocalSearch::greedy_jumper(instance, LocalSearch::get_GJ_Selection_Criterion(file_data.get_string("selection_criterion")), LocalSearch::get_GJ_Neighborhood_Scope(file_data.get_string("neighborhood_scope")), rand, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
    // else if (file_data.get_string("algorithm") == "tabu_search")
    //     Argmax::tabu_search(instance, file_data.get_int("ban_list_size"), file_data.get_int("nb_iteration_max"));
    // else if (file_data.get_string("algorithm") == "one_lambda_search")
    //     Argmax::one_lambda_search(instance, file_data.get_int("nb_mutation_to_test"), file_data.get_int("nb_iteration_max"), true);
    // else if (file_data.get_string("algorithm") == "evolution")
    //     instance = std::move(Argmax::evolution([&instance]() -> std::unique_ptr<ReversibleInstance> { return instance->randomize_clone(); }, Argmax::evolution_parameters(file_data), output_file));
    return 0;
}
void run_on_nk(const FileData& file_data, std::mt19937& rand, std::ostream* output_file = nullptr) {
    std::shared_ptr<NK> nk = nullptr;
    if (file_data.contains_string("instance")) { std::cout << "reading file..." << std::endl; nk = std::shared_ptr<NK>(new NK(file_data.get_string("instance"))); }
    else {
        nk = std::shared_ptr<NK>(new NK(file_data.get_int("N"), file_data.get_int("K"), rand));
    }
    
    FitnessInstance instance = FitnessInstance([&nk](const std::vector<bool>& a) -> float { return nk->evaluate(a); }, nk->get_nb_variables());
    instance.randomize(rand);
    
    std::unique_ptr<ReversibleInstance> temp = std::unique_ptr<FitnessInstance>(new FitnessInstance(std::move(instance)));
    unsigned int used_budget = run(file_data, temp, rand, output_file);
    while (file_data.get_bool("iterate", false) && used_budget < file_data.get_uint("budget", 0))
    {
        std::unique_ptr<ReversibleInstance> temp2 = temp->randomize_clone(rand);
        used_budget = run(file_data, temp2, rand, output_file, used_budget);
        if (temp2->score() > temp->score()) temp = std::move(temp2);
    }
    
    FitnessInstance result = *dynamic_cast<FitnessInstance*>(temp.get());

    std::cout << "max score: " << result << " : " << result.score() << "\n";
}
std::string execute_file(const FileData& file_data, std::mt19937& rand) {
    srand(file_data.get_seed());
    std::ostringstream* output_file = nullptr;
    std::string output_file_path = "";
    if (file_data.get_bool("debug_screen", false)) {
        if (file_data.contains_string("label")) output_file_path = std::regex_replace("./python/data/" + file_data.get_string("label"), std::regex("<timestamp>"), timestamp());
        else                                    output_file_path = "./python/data/" + file_data.get_string("problem") + "_" + file_data.get_string("algorithm") + "_" + timestamp() + ".rundata";

        std::ifstream infile(output_file_path);
        if (infile.good() && !file_data.get_bool("override", false)) {
            std::cout << "execution data already saved in " << output_file_path << std::endl;
            infile.close();
            return output_file_path;
        }
        
        output_file = new std::ostringstream();
    }

    run_on_nk(file_data, rand, output_file);
    
    if (output_file != nullptr) {
        std::filesystem::create_directories(std::filesystem::path(output_file_path).parent_path());
        std::ofstream final_file = std::ofstream(output_file_path);
        if (!final_file.is_open()) {
            std::cerr << "\033[1;31mERROR: cannot open output file at \"" + output_file_path + "\" !\033[0m\n";
            exit(1);
        }

        final_file << output_file->str();
        final_file.close();
        delete output_file;

        std::cout << "execution data saved in " << output_file_path << std::endl;
        return output_file_path;
    }
    return "";
}

void create_all_NK_instances() {
    std::random_device rd;
    std::mt19937 rand(rd());
    for (unsigned int N : { 50, 100, 200, 400, 800, 1000 })
    for (unsigned int K : { 0, 1, 2, 4, 8 })
    for (unsigned int I = 0; I < 10; I++)
    {
        NK nk(N, K, rand);
        nk.save_to_file("./NK/instances/" + std::to_string(N) + "_" + std::to_string(K) + "_" + std::to_string(I) + ".nk");
    }
}

void worker_thread(std::mutex& mutex, std::list<FileData*>& jobs) {
    while (true)
    {
        FileData* file_data;
        {
            std::lock_guard lock(mutex);
            if (jobs.empty()) break;
            file_data = jobs.front();
            jobs.pop_front();
        }
        
        std::mt19937 rand(file_data->get_seed());
        execute_file(*file_data, rand);
    }
}

int main(int argc, char *args[]) {
    // system("chcp 65001");
    std::cout << "\033[1A\r\033[K";
    if (argc < 2) path_message();

    std::string arg1 = args[1];
    if (arg1 == "-visualize_evo" || arg1 == "-v_evo") {
        system("python ./python/evolution_visualizer.py");
        return 0;
    }
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
        NK nk(atoi(args[2]), atoi(args[3]), rand);
        nk.save_to_file(args[4]);
        return 0;
    }

    std::random_device rd;
    auto seed = rd();
    
    auto files_data = generate_file_data(arg1);

    if (files_data.size() == 1) {
        std::mt19937 rand(files_data.front().get_seed());
        std::string output_file_path = execute_file(files_data.front(), rand);
        if (output_file_path != "") {
            if (files_data.front().get_string("algorithm") == "evolution")
                system(("python ./python/evolution_visualizer.py " + output_file_path + " evolution").c_str());
            else
                system(("python ./python/evolution_visualizer.py " + output_file_path + " local_search").c_str());
        }
    }
    else {
        std::mutex mutex;
        std::list<std::thread> threads;
        std::list<FileData*> file_data_ptrs = std::list<FileData*>();
        for (FileData& file_data : files_data) {
            file_data.set_default_seed(seed);
            file_data_ptrs.push_back(&file_data);
        }

        for (unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
        {
            threads.push_back(std::thread(worker_thread, std::ref(mutex), std::ref(file_data_ptrs)));
        }
        for (std::thread& thread : threads) thread.join();
    }
    
    return 0;
}