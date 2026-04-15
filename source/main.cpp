#include <ctime>
#include <chrono>
#include <filesystem>
#include <regex>
#include <thread>
#include <mutex>
#include <sstream>
#include <random>
#include <condition_variable>

#include "algo/argmax.h"
#include "algo/hill_climber/hill_climber.h"
#include "algo/greedy_jumper/greedy_jumper.h"
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

unsigned int run(const FileData& file_data, std::unique_ptr<ReversibleInstance>& instance, unsigned int offset_seed = 0, std::ostream* output_file = nullptr, unsigned int used_budget = 0, bool debug = true) {
    if (file_data.get_string("algorithm") == "hill_climb") {
        LocalSearch::HillClimber algo = LocalSearch::HillClimber(
            file_data.get_seed() + offset_seed,
            LocalSearch::HillClimber::get_Selection_Criterion(file_data.get_string("selection_criterion")),
            debug,
            output_file,
            used_budget==0);
        return algo.improve(instance, file_data.get_int("budget"), used_budget);
    }
    else if (file_data.get_string("algorithm") == "greedy_jumper") {
        LocalSearch::GreedyJumper algo = LocalSearch::GreedyJumper(
            file_data.get_seed() + offset_seed,
            LocalSearch::GreedyJumper::get_Selection_Criterion(file_data.get_string("selection_criterion")),
            LocalSearch::GreedyJumper::get_Neighborhood_Scope(file_data.get_string("neighborhood_scope")),
            debug,
            output_file,
            used_budget==0);
        return algo.improve(instance, file_data.get_int("budget"), used_budget);
    }
    // else if (file_data.get_string("algorithm") == "tabu_search")
    //     Argmax::tabu_search(instance, file_data.get_int("ban_list_size"), file_data.get_int("nb_iteration_max"));
    // else if (file_data.get_string("algorithm") == "one_lambda_search")
    //     Argmax::one_lambda_search(instance, file_data.get_int("nb_mutation_to_test"), file_data.get_int("nb_iteration_max"), true);
    // else if (file_data.get_string("algorithm") == "evolution")
    //     instance = std::move(Argmax::evolution([&instance]() -> std::unique_ptr<ReversibleInstance> { return instance->randomize_clone(); }, Argmax::evolution_parameters(file_data), output_file));
    return 0;
}
void run_on_nk(const FileData& file_data, std::ostream* output_file = nullptr, bool debug = true) {
    std::shared_ptr<NK> nk = nullptr;
    if (file_data.contains_string("instance")) {
        if (debug) std::cout << "reading file..." << std::endl;
        nk = std::shared_ptr<NK>(new NK(file_data.get_string("instance")));
    }
    else {
        std::mt19937 rand(file_data.get_seed());
        nk = std::shared_ptr<NK>(new NK(file_data.get_int("N"), file_data.get_int("K"), rand));
    }

    FitnessInstance instance = FitnessInstance([&nk](const std::vector<bool>& a) -> float { return nk->evaluate(a); }, nk->get_nb_variables());
    {
        std::mt19937 start_point_randomizer(file_data.get_seed());
        instance.randomize(start_point_randomizer);
    }
    
    std::unique_ptr<ReversibleInstance> temp = std::unique_ptr<FitnessInstance>(new FitnessInstance(std::move(instance)));
    unsigned int used_budget = run(file_data, temp, 0, output_file, 0, debug);
    int iteration = 0;
    while (file_data.get_bool("iterate", false) && used_budget < file_data.get_uint("budget", 0))
    {
        iteration += 1;
        std::mt19937 start_point_randomizer(file_data.get_seed() + iteration);
        std::unique_ptr<ReversibleInstance> temp2 = temp->randomize_clone(start_point_randomizer);

        used_budget = run(file_data, temp2, iteration, output_file, used_budget, debug);
        if (temp2->score() > temp->score()) temp = std::move(temp2);
    }
    
    if (debug) {    
        FitnessInstance result = *dynamic_cast<FitnessInstance*>(temp.get());
        
        std::cout << "max score: " << result << " : " << result.score() << "\n";
    }
}
std::string execute_file(const FileData& file_data, bool debug = true) {
    srand(file_data.get_seed());
    std::ostringstream* output_file = nullptr;
    std::string output_file_path = "";
    if (file_data.get_bool("debug_screen", false)) {
        if (file_data.contains_string("label")) output_file_path = std::regex_replace("./python/data/" + file_data.get_string("label"), std::regex("<timestamp>"), timestamp());
        else                                    output_file_path = "./python/data/" + file_data.get_string("problem") + "_" + file_data.get_string("algorithm") + "_" + timestamp() + ".rundata";

        std::ifstream infile(output_file_path);
        if (infile.good() && !file_data.get_bool("override", false)) {
            if (debug) std::cout << "execution data already saved in " << output_file_path << std::endl;
            infile.close();
            return output_file_path;
        }
        
        output_file = new std::ostringstream();
    }

    run_on_nk(file_data, output_file, debug);
    
    if (output_file != nullptr) {
        std::filesystem::create_directories(std::filesystem::path(output_file_path).parent_path());
        std::ofstream final_file = std::ofstream(output_file_path);
        if (!final_file.is_open()) {
            if (debug) std::cerr << "\033[1;31mERROR: cannot open output file at \"" + output_file_path + "\" !\033[0m\n";
            exit(1);
        }

        final_file << output_file->str();
        final_file.close();
        delete output_file;

        if (debug) std::cout << "execution data saved in " << output_file_path << std::endl;
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

void worker_thread(std::mutex& mutex, std::condition_variable& condition, std::list<FileData*>& jobs) {
    while (true)
    {
        FileData* file_data;
        {
            std::lock_guard lock(mutex);
            if (jobs.empty()) break;
            file_data = jobs.front();
            jobs.pop_front();
        }
        
        execute_file(*file_data, false);

        condition.notify_all();
    }
}
float inv_lerp(float from, float to, float value){
    return (value - from) / (to - from);
}
float lerp(float from, float to, float value){
    return ((1 - value) * from) + (value * to);
}
/// @brief prints a progress
/// @param value the fill value from 0 to 1
/// @param char_count the size of the progress bar
void cout_bar(float value, int char_count = 100) {
    std::string chars[] {"█", "▉", "▊", "▋", "▌", "▍", "▎", "▏"};
    std::cout << "[";
    for (int i = 0; i < char_count; i++)
    {
        if (i < value * char_count) std::cout << "█";
        else if (i + 1 > value * char_count) std::cout << " ";
        else {
            float v = inv_lerp(i, i+1, value * char_count);
            std::cout << chars[(int)lerp(0, 8, v)];
        }
    }
    std::cout << "] (" << ((int)(value * 100)) << "%)";
}
void progress_bar(std::mutex& mutex, std::condition_variable& condition, std::list<FileData*>& jobs, unsigned int total_job_count) {
    unsigned int job_count = total_job_count;

    while (job_count != 0)
    {
        std::cout << "\r\033[2K";
        cout_bar((float)(total_job_count - job_count) / total_job_count);
        std::cout << "\t" << (total_job_count - job_count) << " / " << total_job_count << std::flush;

        std::unique_lock<std::mutex> lock(mutex);
        while (jobs.size() == job_count) condition.wait(lock);
        job_count = jobs.size();
        lock.unlock();
    }
    std::cout << "\r\033[2K";
    cout_bar(1);
    std::cout << "\t" << total_job_count << " / " << total_job_count << std::endl;
}

int main(int argc, char *args[]) {
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
        files_data.front().set_default_seed(seed);
        std::string output_file_path = execute_file(files_data.front());
        if (output_file_path != "") {
            if (files_data.front().get_string("algorithm") == "evolution")
                system(("python ./python/evolution_visualizer.py " + output_file_path + " evolution").c_str());
            else
                system(("python ./python/evolution_visualizer.py " + output_file_path + " local_search").c_str());
        }
    }
    else {
        std::mutex mutex;
        std::condition_variable condition;
        std::list<std::thread> threads;
        std::list<FileData*> file_data_ptrs = std::list<FileData*>();
        for (FileData& file_data : files_data) {
            file_data.set_default_seed(seed);
            file_data_ptrs.push_back(&file_data);
        }

        // launch threads
        for (unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
        {
            threads.push_back(std::thread(worker_thread, std::ref(mutex), std::ref(condition), std::ref(file_data_ptrs)));
        }

        // progress bar
        threads.push_back(std::thread(progress_bar, std::ref(mutex), std::ref(condition), std::ref(file_data_ptrs), files_data.size()));

        // join threads
        for (std::thread& thread : threads) thread.join();
    }
    
    return 0;
}