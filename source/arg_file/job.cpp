#include "job.h"
#include <regex>
#include <filesystem>
#include "../algo/local_search_factory.h"
#include "../instance/fitness_instance.h"

Job::Job(RunParameters* parameters, unsigned int algo_index, unsigned int instance_index): parameters(parameters), algo_index(algo_index), instance_index(instance_index) {}

const Parameters& Job::get_run_infos() const { return *parameters; }
const Parameters& Job::get_algo_parameters() const { return parameters->get_algos()[algo_index]; }
const Parameters& Job::get_instance_parameters() const { return parameters->get_instances()[instance_index]; }

std::string timestamp() {
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof (buffer), "%Hh%M_%d_%m_%Y", timeinfo);
    return std::string (buffer);
}
std::string replace(const std::string& text, const std::string& key, const std::string& replacement) {
    return std::regex_replace(text, std::regex(key), replacement);
}
std::string Job::get_output_file_path() const {
    const Parameters& algo = get_algo_parameters();
    const Parameters& instance = get_instance_parameters();

    if (parameters->get_bool("save_run", false)) {
        if (instance.contains_string("save_path") && algo.contains_string("label"))
            return replace(replace(instance.get_string("save_path"), "<label>", algo.get_string("label")), "<timestamp>", timestamp());
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
std::pair<std::string, std::ostringstream*> Job::get_output_pair(bool debug) const {
    if (parameters->get_bool("save_run", false) == false) return {"", nullptr};

    std::string output_file_path = get_output_file_path();
    if (do_output_data_exists(output_file_path) && parameters->get_bool("override", false) == false) {
        if (debug) std::cout << "execution data already saved in " << output_file_path << std::endl;
        return {output_file_path, nullptr};
    }

    return {output_file_path, new std::ostringstream()};
}

std::shared_ptr<NK> Job::get_problem(bool debug) const {
    const Parameters& instance_parameters = get_instance_parameters();

    if (instance_parameters.get_string("problem") == "NK") {
        if (instance_parameters.contains_string("instance_path")) {
            if (debug) std::cout << "reading file..." << std::endl;
            return std::make_shared<NK>(instance_parameters.get_string("instance_path"));
        }
        else {
            if (debug) std::cout << "creating problem..." << std::endl;
            std::mt19937 rand(parameters->get_seed());
            return std::make_shared<NK>(instance_parameters.get_int("N"), instance_parameters.get_int("K"), rand);
        }
    }
    return nullptr;
}

std::string Job::execute_job(bool debug) const {
    std::pair<std::string, std::ostringstream*> output_pair = get_output_pair(debug);
    if (output_pair.first != "" && output_pair.second == nullptr) return output_pair.first; // execution data already saved

    // get algo
    std::shared_ptr<LocalSearch::LocalSearchAlgo> algo = LocalSearch::create_local_search_algo(get_algo_parameters());
    algo->set_debug(debug)
        ->set_seed(std::make_shared<std::mt19937>(parameters->get_seed()))
        ->set_output(output_pair.second);
    
    // get problem
    std::shared_ptr<NK> nk = get_problem(debug);

    // create instance
    std::mt19937 rand = std::mt19937(parameters->get_seed());
    std::unique_ptr<ReversibleInstance> instance = FitnessInstance([&nk](const std::vector<bool>& a) -> float { return nk->evaluate(a); }, nk->get_nb_variables()).randomize_clone(rand);

    // run
    BudgetHelper budget(parameters->get_int("budget"));
    algo->run(instance, budget);

    // run results
    if (debug) {
        FitnessInstance result = *dynamic_cast<FitnessInstance*>(instance.get());
        
        std::cout << "max score: " << result << " : " << result.score() << "\n";
    }

    // save run data to real file
    if (output_pair.second != nullptr) {
        std::filesystem::create_directories(std::filesystem::path(output_pair.first).parent_path());
        std::ofstream final_file = std::ofstream(output_pair.first);
        if (!final_file.is_open()) {
            if (debug) std::cerr << "\033[1;31mERROR: cannot open output file at \"" + output_pair.first + "\" !\033[0m\n";
            exit(1);
        }

        final_file << output_pair.second->str();
        final_file.close();
        delete output_pair.second;

        if (debug) std::cout << "execution data saved in " << output_pair.first << std::endl;
        return output_pair.first;
    }
    return "";
}
