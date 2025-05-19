#include "source/argmax.h"
#include "source/file_data.h"
#include <ctime>
#include <chrono>

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
    std::cerr << "\033[1;31mIf you want to run an algorithm you need to put a path to a file with all the informations\nIf you want to launch the visualizer please use -visualize or -v\n\033[0m";
    exit(1);
}

void run(const FileData& file_data, std::unique_ptr<Instance>& instance, std::ofstream* output_file = nullptr) {
    if (file_data.get_string("algorithm") == "hill_climb") {
        Argmax::hill_climb(instance, file_data.get_int("nb_iteration_max"));
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
}
void run_on_sat(const FileData& file_data, std::ofstream* output_file = nullptr) {
    std::cout << "reading file..." << std::endl;
    std::shared_ptr<Formule> f = std::shared_ptr<Formule>(new Formule(file_data.get_string("instance")));
    
    Solution solution(f);
    solution.randomize();
    std::unique_ptr<Instance> temp = solution.clone();
    run(file_data, temp, output_file);
    Solution result = *dynamic_cast<Solution*>(temp.get());

    std::cout << "max score: " << result << " : " << int(result.score()) << " out of " << f->get_nb_clauses() << "\n";
}
void run_on_fa(const FileData& file_data, std::ofstream* output_file = nullptr) {
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
    run(file_data, temp, output_file);
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

int main(int argc, char *args[]) {
    system("chcp 65001");
    std::cout << "\033[1A\r\033[K";
    if (argc < 2) path_message();

    std::string arg1 = args[1];
    if (arg1 == "-visualize" || arg1 == "-v") {
        system("python ./python/data_visualizer.py");
        return 0;
    }
    else if (arg1 == "-test" || arg1 == "-t") {
        test_FA_hand(argc, 2, args);
        return 0;
    }

    srand(time(NULL));
    
    FileData file_data = FileData(arg1);

    std::ofstream* output_file = nullptr;
    if (file_data.get_bool("debug_screen", false)) {
        output_file = new std::ofstream("./temp.rundata");
        if (!(*output_file).is_open()) {
            std::cerr << "\033[1;31mERROR: cannot open output file at \"./temp.rundata\" !\033[0m\n";
            exit(1);
        }
    }

    if (file_data.get_string("problem") == "SAT") run_on_sat(file_data, output_file);
    if (file_data.get_string("problem") == "FA") run_on_fa(file_data, output_file);
    
    if (output_file != nullptr) {
        (*output_file).close();
        delete output_file;

        std::string output_file_path = "";
        if (file_data.contains_string("label")) output_file_path = "./python/data/" + file_data.get_string("label") + "_" + timestamp() + ".rundata";
        else                                    output_file_path = "./python/data/" + file_data.get_string("problem") + "_" + file_data.get_string("algorithm") + "_" + timestamp() + ".rundata";
        std::ofstream final_file = std::ofstream(output_file_path);
        if (!final_file.is_open()) {
            std::cerr << "\033[1;31mERROR: cannot open output file at \"" + output_file_path + "\" !\033[0m\n";
            exit(1);
        }

        std::ifstream temp_file = std::ifstream("./temp.rundata");
        if (!temp_file.is_open()) {
            std::cerr << "\033[1;31mERROR: cannot open output file at \"./temp.rundata\" !\033[0m\n";
            exit(1);
        }

        std::string line; 
        while (std::getline(temp_file, line)) final_file << line << "\n";

        final_file.close();
        temp_file.close();
        std::remove("./temp.rundata");

        std::cout << "data saved in " << output_file_path << std::endl;
        system(("python ./python/data_visualizer.py " + output_file_path).c_str());
    }

    return 0;
}