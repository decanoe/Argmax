#include "source/argmax.h"
#include "source/file_data.h"
#include <ctime>

#include "sat_specific/formule.h"
#include "sat_specific/solution.h"

#include "FA/deck.h"
#include "FA/hand.h"

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

void run_on_sat(const FileData& file_data, std::ofstream* output_file = nullptr) {
    std::cout << "reading file..." << std::endl;
    std::shared_ptr<Formule> f = std::shared_ptr<Formule>(new Formule(file_data.get_string("instance")));
    
    Solution solution(f);
    solution.randomize();
    std::unique_ptr<Instance> temp;
    
    if (file_data.get_string("algorithm") == "hill_climb") {
        temp = Argmax::hill_climb(solution.clone(), file_data.get_int("nb_iteration_max"));
    }
    else if (file_data.get_string("algorithm") == "hill_climb_ban") {
        temp = Argmax::tabu_search(solution.clone(), file_data.get_int("ban_list_size"), file_data.get_int("nb_iteration_max"));
    }
    else if (file_data.get_string("algorithm") == "evolution") {
        temp = Argmax::evolution([solution]() -> std::unique_ptr<Instance> { return solution.randomize_clone(); }, Argmax::evolution_parameters(file_data), output_file);
    }
    Solution result = *dynamic_cast<Solution*>(temp.get());

    std::cout << "max score: " << result << " : " << int(result.score()) << " out of " << f->get_nb_clauses() << "\n";
}
void run_on_fa(const FileData& file_data, std::ofstream* output_file = nullptr) {
    std::cout << "reading files..." << std::endl;
    std::shared_ptr<Deck> deck = std::make_shared<Deck>(file_data.get_string("cards"), file_data.get_string("sanctuaries"));
    
    Hand h(deck);
    h.randomize();
    std::unique_ptr<Instance> temp;
    if (file_data.get_string("algorithm") == "hill_climb") {
        temp = Argmax::hill_climb(h.clone(), file_data.get_int("nb_iteration_max"));
    }
    else if (file_data.get_string("algorithm") == "hill_climb_ban") {
        temp = Argmax::tabu_search(h.clone(), file_data.get_int("ban_list_size"), file_data.get_int("nb_iteration_max"));
    }
    else if (file_data.get_string("algorithm") == "evolution") {
        temp = Argmax::evolution([h]() -> std::unique_ptr<Instance> { return h.randomize_clone(); }, Argmax::evolution_parameters(file_data), output_file);
    }
    Hand result = *dynamic_cast<Hand*>(temp.get());

    std::cout << "max score: " << int(result.score()) << " points with hand\n";
    result.pretty_cout(std::cout);
}

int main(int argc, char *args[]) {
    system("chcp 65001");
    std::cout << "\033[1A\r\033[K";
    if (argc < 2) path_message();

    if (args[1] == "-visualize" || args[1] == "-v") {
        system("python ./python/data_visualizer.py");
        return 0;
    }

    srand(time(NULL));
    
    FileData file_data = FileData(args[1]);

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
    
    if ((*output_file).is_open()) {
        (*output_file).close();
        delete output_file;

        std::string output_file_path = "./python/data/FA_" + file_data.get_string("algorithm") + "_" + timestamp() + ".rundata";
        std::ofstream final_file = std::ofstream(output_file_path);
        if (!final_file.is_open()) {
            std::cerr << "\033[1;31mERROR: cannot open output file at \"./temp.rundata\" !\033[0m\n";
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