#include "source/argmax.h"
#include "source/file_data.h"
#include <time.h>

#include "sat_specific/formule.h"
#include "sat_specific/solution.h"

#include "FA/deck.h"
#include "FA/hand.h"

void path_message() {
    std::cerr << "\033[1;31mYou need to put a path to a file with informations to run\n\033[0m";
    exit(1);
}

void run_on_sat(const FileData& file_data) {
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
        temp = Argmax::evolution([solution]() -> std::unique_ptr<Instance> { return solution.randomize_clone(); }, Argmax::evolution_parameters(file_data));
    }
    Solution result = *dynamic_cast<Solution*>(temp.get());

    std::cout << "max score: " << result << " : " << int(result.score()) << " out of " << f->get_nb_clauses() << "\n";
}
void run_on_fa(const FileData& file_data) {
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
        temp = Argmax::evolution([h]() -> std::unique_ptr<Instance> { return h.randomize_clone(); }, Argmax::evolution_parameters(file_data));
    }
    Hand result = *dynamic_cast<Hand*>(temp.get());

    std::cout << "max score: " << int(result.score()) << " points with hand\n";
    result.pretty_cout(std::cout);

    if (file_data.get_bool("debug_screen", false)) system("python ./python/data_visualizer.py");
}

int main(int argc, char *args[]) {
    system("chcp 65001");
    std::cout << "\033[1A\r\033[K";
    if (argc < 2) path_message();

    srand(time(NULL));
    
    FileData file_data = FileData(args[1]);

    if (file_data.get_string("problem") == "SAT") run_on_sat(file_data);
    if (file_data.get_string("problem") == "FA") run_on_fa(file_data);

    return 0;
}