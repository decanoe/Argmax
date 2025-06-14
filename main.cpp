#include "source/argmax.h"
#include "source/local_search.h"
#include "source/file_data.h"
#include "source/fitness_instance.h"
#include <ctime>
#include <chrono>
#include <filesystem>
#include <regex>
#include <thread>
#include <sstream>

#include "NK/nk.h"

#include "SAT/formule.h"
#include "SAT/solution.h"

#include "FA/deck.h"
#include "FA/hand.h"
#include "FA/people.h"

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

unsigned int run(const FileData& file_data, std::unique_ptr<Instance>& instance, std::ostream* output_file = nullptr, unsigned int used_budget = 0) {
    if (file_data.get_string("algorithm") == "hill_climb") {
        if (file_data.get_string("hc_choice") == "random")
            return LocalSearch::hill_climb_random(instance, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
        else if (file_data.get_string("hc_choice") == "first")
            return LocalSearch::hill_climb_first(instance, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
        else if (file_data.get_string("hc_choice") == "cycle")
            return LocalSearch::hill_climb_cycle(instance, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
        else if (file_data.get_string("hc_choice") == "least")
            return LocalSearch::hill_climb_least(instance, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
        else if (file_data.get_string("hc_choice") == "best")
            return LocalSearch::hill_climb_best(instance, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
    }
    else if (file_data.get_string("algorithm") == "greedy") {
        if (file_data.get_string("greedy_type") == "all") {
            if (file_data.get_string("greedy_choice") == "first")
                return LocalSearch::hill_climb_greedy_all_first(instance, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
            else if (file_data.get_string("greedy_choice") == "best")
                return LocalSearch::hill_climb_greedy_all_best(instance, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
            else if (file_data.get_string("greedy_choice") == "least")
                return LocalSearch::hill_climb_greedy_all_least(instance, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
        }
        else if (file_data.get_string("greedy_type") == "improve") {
            if (file_data.get_string("greedy_choice") == "first")
                return LocalSearch::hill_climb_greedy_first(instance, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
            else if (file_data.get_string("greedy_choice") == "best")
                return LocalSearch::hill_climb_greedy_best(instance, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
            else if (file_data.get_string("greedy_choice") == "least")
                return LocalSearch::hill_climb_greedy_least(instance, file_data.get_int("budget"), used_budget, output_file, used_budget==0);
        }
    }
    else if (file_data.get_string("algorithm") == "tabu_search") {
        Argmax::tabu_search(instance, file_data.get_int("ban_list_size"), file_data.get_int("nb_iteration_max"));
    }
    else if (file_data.get_string("algorithm") == "one_lambda_search") {
        Argmax::one_lambda_search(instance, file_data.get_int("nb_mutation_to_test"), file_data.get_int("nb_iteration_max"), true);
    }
    else if (file_data.get_string("algorithm") == "evolution") {
        instance = std::move(Argmax::evolution([&instance]() -> std::unique_ptr<Instance> { return instance->randomize_clone(); }, Argmax::evolution_parameters(file_data), output_file));
    }

    return 0;
}
void run_on_sat(const FileData& file_data, std::ostream* output_file = nullptr) {
    std::cout << "reading file..." << std::endl;
    std::shared_ptr<Formule> f = std::shared_ptr<Formule>(new Formule(file_data.get_string("instance")));
    
    FitnessInstance instance = FitnessInstance([&f](const std::vector<bool>& a) -> float { return f->count_valid_clauses(a); }, f->get_nb_variables());
    instance.randomize();

    std::unique_ptr<Instance> temp = instance.clone();
    unsigned int used_budget = run(file_data, temp, output_file);
    while (file_data.get_bool("iterate", false) && used_budget < file_data.get_uint("budget", 0))
    {
        std::unique_ptr<Instance> temp2 = temp->randomize_clone();
        used_budget = run(file_data, temp2, output_file, used_budget);
        if (temp2->score() > temp->score()) temp = std::move(temp2);
    }
    FitnessInstance result = *dynamic_cast<FitnessInstance*>(temp.get());

    std::cout << "max score: " << result << " : " << int(result.score()) << " out of " << f->get_nb_clauses() << "\n";
}
void run_on_nk(const FileData& file_data, std::ostream* output_file = nullptr) {
    std::shared_ptr<NK> nk = nullptr;
    if (file_data.contains_string("instance")) { std::cout << "reading file..." << std::endl; nk = std::shared_ptr<NK>(new NK(file_data.get_string("instance"))); }
    else                                        nk = std::shared_ptr<NK>(new NK(file_data.get_int("N"), file_data.get_int("K")));
    
    FitnessInstance instance = FitnessInstance([&nk](const std::vector<bool>& a) -> float { return nk->evaluate(a); }, nk->get_nb_variables());
    instance.randomize();
    
    std::unique_ptr<Instance> temp = instance.clone();
    unsigned int used_budget = run(file_data, temp, output_file);
    while (file_data.get_bool("iterate", false) && used_budget < file_data.get_uint("budget", 0))
    {
        std::unique_ptr<Instance> temp2 = temp->randomize_clone();
        used_budget = run(file_data, temp2, output_file, used_budget);
        if (temp2->score() > temp->score()) temp = std::move(temp2);
    }
    
    FitnessInstance result = *dynamic_cast<FitnessInstance*>(temp.get());

    std::cout << "max score: " << result << " : " << result.score() << "\n";
}
void run_on_fa(const FileData& file_data, std::ostream* output_file = nullptr) {
    std::cout << "reading files..." << std::endl;
    std::shared_ptr<Deck> deck = std::make_shared<Deck>(file_data.get_string("cards"), file_data.get_string("sanctuaries"));

    if (file_data.contains_string("subset")) deck->keep_only_subset(Deck::read_subset_file(file_data.get_string("subset")));
    deck->display_deck(std::cout);
    
    Hand h(deck);

    /*double t = 0;
    int count = 0;
    for (size_t i = 0; i < 1024; i++)
    {
        auto start = std::chrono::system_clock::now();

        h.randomize();
        h.score();

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_time = end - start;
        double elapsed_seconds = elapsed_time.count();
        t += elapsed_seconds;
        count++;
    }
    std::cout << "avg time per hand: " << t / count << std::endl;
    exit(-1);*/

    h.randomize();
    std::unique_ptr<Instance> temp = h.clone();
    unsigned int used_budget = run(file_data, temp, output_file);
    while (file_data.get_bool("iterate", false) && used_budget < file_data.get_uint("budget", 0))
    {
        std::unique_ptr<Instance> temp2 = temp->randomize_clone();
        used_budget = run(file_data, temp2, output_file, used_budget);
        if (temp2->score() > temp->score()) temp = std::move(temp2);
    }
    Hand result = *dynamic_cast<Hand*>(temp.get());

    std::cout << "max score: " << int(result.score()) << " points with hand\n";
    result.pretty_cout(std::cout);
}

void create_valid_FA_subset(unsigned int max_tries = 1024) {
    std::shared_ptr<Deck> deck = std::make_shared<Deck>("./FA/data/cards.txt", "./FA/data/sanctuary.txt");

    unsigned int people_count = 8;
    unsigned int sanct_count = 7;

    std::vector<unsigned int> peoples = std::vector<unsigned int>();
    std::vector<unsigned int> sancts = std::vector<unsigned int>();
    
    for (size_t i = 0; i < max_tries; i++)
    {
        Card::HandInfo hinfo = Card::HandInfo();
        for (size_t i = 0; i < people_count; i++)
        {
            peoples.push_back(RandomUtils::get_index(deck->get_people_count(), peoples));
            hinfo.add(*(deck->get_people(peoples.back())));
        }
        for (size_t i = 0; i < sanct_count; i++)
        {
            sancts.push_back(RandomUtils::get_index(deck->get_sanctuary_count(), sancts));
            hinfo.add(*(deck->get_sanctuary(sancts.back())));
        }

        bool valid = true;
        for (unsigned int index : peoples)
        {
            if (((People*)deck->get_people(index).get())->cost_paid(hinfo)) continue;
            valid = false;
            break;
        }
        if (valid) {
            for (unsigned int index : peoples) std::cout << deck->get_people(index)->get_index() << " ";
            for (unsigned int index : sancts) std::cout << deck->get_sanctuary(index)->get_index() << " ";
            std::cout << "\n";
            return;
        }
    }
    std::cout << "No subset found\n";
    return;
}
void test_FA_hand(int argc, int first_card, char *args[]) {
    std::cout << "reading files..." << std::endl;
    std::shared_ptr<Deck> deck = std::make_shared<Deck>("./FA/data/cards.txt", "./FA/data/sanctuary.txt");

    std::vector<unsigned int> peoples = std::vector<unsigned int>();
    std::set<unsigned int> sanctuaries = std::set<unsigned int>();

    for (int i = first_card; i < argc; i++)
    {
        unsigned int index = std::stoi(args[i]);
        if (index > 100) sanctuaries.insert(deck->get_sanctuary_index_by_index(index));
        else             peoples.push_back(deck->get_people_index_by_index(index));
    }

    Hand h(deck, peoples, sanctuaries);
    h.pretty_cout(std::cout);
    std::cout << "score: " << int(h.score()) << " points with hand\n";
}

std::string execute_file(const FileData file_data) {
    srand(file_data.get_seed());
    std::ostringstream* output_file = nullptr;
    std::string output_file_path = "";
    if (file_data.get_bool("debug_screen", false)) {
        if (file_data.contains_string("label")) output_file_path = std::regex_replace("./python/data/" + file_data.get_string("label"), std::regex("<timestamp>"), timestamp());
        else                                    output_file_path = "./python/data/" + file_data.get_string("problem") + "_" + file_data.get_string("algorithm") + "_" + timestamp() + ".rundata";

        std::ifstream infile(output_file_path);
        if (infile.good() && !file_data.get_bool("override", false)) return output_file_path;
        
        output_file = new std::ostringstream();
    }

    if (file_data.get_string("problem") == "SAT") run_on_sat(file_data, output_file);
    if (file_data.get_string("problem") == "FA") run_on_fa(file_data, output_file);
    if (file_data.get_string("problem") == "NK") run_on_nk(file_data, output_file);
    
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

        std::cout << "data saved in " << output_file_path << std::endl;
        return output_file_path;
    }
    return "";
}

int main(int argc, char *args[]) {
    system("chcp 65001");
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
    else if (arg1 == "-test" || arg1 == "-t") {
        test_FA_hand(argc, 2, args);
        return 0;
    }
    else if (arg1 == "-create_nk" || arg1 == "-nk") {
        if (argc < 5) {
            std::cerr << "\033[1;31mIf you want to create an nk instance you need to put a N, a K and a path to save the instance.\n\033[0m";
            exit(1);
        }
        srand(time(NULL));

        // for (unsigned int N : { 50, 100, 200, 400, 800, 1000 })
        // for (unsigned int K : { 0, 1, 2, 4, 8 })
        // for (unsigned int I = 0; I < 10; I++)
        // {
        //     NK nk(N, K);
        //     nk.save_to_file("./NK/instances/" + std::to_string(N) + "_" + std::to_string(K) + "_" + std::to_string(I) + ".nk");
        // }
        // exit(0);
        
        NK nk(atoi(args[2]), atoi(args[3]));
        nk.save_to_file(args[4]);
        return 0;
    }

    auto seed = time(NULL);
    
    auto files_data = generate_file_data(arg1);
    std::list<std::thread> threads;
    std::string output_file_path = "";
    for (FileData& file_data : files_data)
    {
        file_data.set_default_seed(seed);
        std::string output_file_path = execute_file(file_data);

        // threads.push_back(std::thread(execute_file, file_data));
    }
    // while (threads.size() != 0) {
    //     threads.back().join();
    //     threads.pop_back();
    // }
    if (files_data.size() == 1) {
        if (files_data.front().get_string("algorithm") == "evolution")
            system(("python ./python/evolution_visualizer.py " + output_file_path + " evolution").c_str());
        else
            system(("python ./python/evolution_visualizer.py " + output_file_path + " local_search").c_str());
    }
    return 0;
}