#include <ctime>
#include <chrono>
#include <filesystem>
#include <regex>
#include <thread>
#include <mutex>
#include <sstream>
#include <random>
#include <condition_variable>

#include "algo/greedy_jumper/greedy_jumper.h"
#include "algo/local_search_factory.h"
#include "arg_file/file_data.h"
#include "instance/fitness_instance.h"
#include "problem/nk.h"

void path_message() {
    std::cerr << "\033[1;31mIf you want to run an algorithm you need to put a path to a file with all the informations\n\033[0m";
    std::cerr << "\033[1;31mIf you want to launch the visualizer please use -visualize or -v\n\033[0m";
    std::cerr << "\033[1;31mIf you want to test a Faraway hand please use -test or -t followed by all the card indices\n\033[0m";
    std::cerr << "\033[1;31mIf you want to create a NK instance please use -create_nk or -nk followed by the N and the K wanted and a path to save the instance\n\033[0m";
    exit(1);
}

std::string timestamp() {
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof (buffer), "%Hh%M_%d_%m_%Y", timeinfo);
    return std::string (buffer);
}
std::string get_output_file_path(const FileData& file_data) {
    if (file_data.get_bool("debug_screen", false)) {
        if (file_data.contains_string("label")) return std::regex_replace("./python/data/" + file_data.get_string("label"), std::regex("<timestamp>"), timestamp());
        else                                    return "./python/data/" + file_data.get_string("problem") + "_" + file_data.get_string("algorithm") + "_" + timestamp() + ".rundata";
    }
    return "";
}
bool do_output_data_exists(const std::string& output_file_path) {
    if (output_file_path == "") return false;

    std::ifstream file(output_file_path);
    if (file.good()) {
        file.close();
        return true;
    }
    return false;
}
std::pair<std::string, std::ostringstream*> get_data_output_pair(const FileData& file_data, bool debug = true) {
    if (file_data.get_bool("debug_screen", false) == false) return {"", nullptr};

    std::string output_file_path = get_output_file_path(file_data);
    if (do_output_data_exists(output_file_path) && file_data.get_bool("override", false) == false) {
        if (debug) std::cout << "execution data already saved in " << output_file_path << std::endl;
        return {output_file_path, nullptr};
    }

    return {output_file_path, new std::ostringstream()};
}

std::shared_ptr<NK> get_nk_problem(const FileData& file_data, bool debug = true) {
    std::shared_ptr<NK> nk = nullptr;
    if (file_data.contains_string("instance")) {
        if (debug) std::cout << "reading file..." << std::endl;
        return std::make_shared<NK>(file_data.get_string("instance"));
    }
    else {
        if (debug) std::cout << "creating problem..." << std::endl;
        std::mt19937 rand(file_data.get_seed());
        return std::make_shared<NK>(file_data.get_int("N"), file_data.get_int("K"), rand);
    }
}

std::string execute_file(const FileData& file_data, bool debug = true) {
    std::pair<std::string, std::ostringstream*> data_output_pair = get_data_output_pair(file_data, debug);
    if (data_output_pair.first != "" && data_output_pair.second == nullptr) return data_output_pair.first; // execution data already saved

    // get algo
    std::shared_ptr<LocalSearch::LocalSearchAlgo> algo = LocalSearch::create_local_search_algo(file_data);
    algo->set_debug(debug)
        ->set_seed(std::make_shared<std::mt19937>(file_data.get_seed()))
        ->set_output(data_output_pair.second);
    
    // get problem
    std::shared_ptr<NK> nk = get_nk_problem(file_data, debug);

    // create instance
    std::mt19937 rand = std::mt19937(file_data.get_seed());
    std::unique_ptr<ReversibleInstance> instance = FitnessInstance([&nk](const std::vector<bool>& a) -> float { return nk->evaluate(a); }, nk->get_nb_variables()).randomize_clone(rand);

    // run
    algo->improve(instance, file_data.get_int("budget"));

    // run results
    if (debug) {
        FitnessInstance result = *dynamic_cast<FitnessInstance*>(instance.get());
        
        std::cout << "max score: " << result << " : " << result.score() << "\n";
    }

    // save run data to real file
    if (data_output_pair.second != nullptr) {
        std::filesystem::create_directories(std::filesystem::path(data_output_pair.first).parent_path());
        std::ofstream final_file = std::ofstream(data_output_pair.first);
        if (!final_file.is_open()) {
            if (debug) std::cerr << "\033[1;31mERROR: cannot open output file at \"" + data_output_pair.first + "\" !\033[0m\n";
            exit(1);
        }

        final_file << data_output_pair.second->str();
        final_file.close();
        delete data_output_pair.second;

        if (debug) std::cout << "execution data saved in " << data_output_pair.first << std::endl;
        return data_output_pair.first;
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
    std::cout << "\t" << total_job_count << " / " << total_job_count << " (waiting for the last job batch to end)" << std::endl;
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
        return 0;
    }

    if (argc > 2 && std::string(args[2]) == "-t") {
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
        return 0;
    }

    for (FileData& file_data : files_data) {
        file_data.set_default_seed(seed);
        execute_file(file_data, true);
    }
    
    return 0;
}