#include "source/argmax.h"
#include "sat_specific/formule.h"
#include "sat_specific/solution.h"

#include "FA/deck.h"
#include "FA/hand.h"
#include <time.h>
// chcp 65001
#define __max(a,b) (((a) > (b)) ? (a) : (b))

void path_message() {
    std::cerr << "\033[1;31mYou need to put a path to a cnf (or txt) file\n\033[0m";
    exit(1);
}
void algo_message(){
    std::cerr << "\033[1;31mYou need to put an algorithm to run :\n\
\t-hc         \tto use hill_climb algorithm\n\
\t-hc_ban     \tto use hill_climb algorithm with ban list\n\
\t-evo_simple \tto use the simple evolution algorithm\n\
\t-evo        \tto use the evolution algorithm\n\n\
\t-fa -<algo> \tto run with fa problem\n\033[0m";
    exit(2);
}

void run_hill_climb(int argc, char *args[]) {
    std::shared_ptr<Formule> f = std::shared_ptr<Formule>(new Formule(args[1]));
    std::cout << "file loaded\n";
    
    Solution solution(f);
    
    Solution best = solution;
    unsigned int max = 0;
    int percent = 0;
    int nb_iter = 512;
    
    solution.randomize();
    for (int i = 0; i <= nb_iter; i++)
    {
        solution.randomize();
        std::unique_ptr<Instance> temp = Argmax::hill_climb(solution.clone());
        Solution result = *dynamic_cast<Solution*>(temp.get());
    
        unsigned int count = result.score();
        if (count > max) {
            max = count;
            best = result;
        }

        if (max >= f->get_nb_clauses()) break;

        int p = 100 * (float)i / nb_iter;
        if (p != percent) {
            percent = p;
            std::string t = "\rprogress: " + std::to_string(p);
            if (p < 10) t += " ";
            t += "% [";
            for (int i = 0; i < p / 10; i++) t += "=";//"▆";
            for (int i = p / 10; i < 10; i++) t += " ";
            
            std::cout << t + "]          ";
        }
    }
    std::cout << "\nmax score: " << best << " : " << max << " out of " << f->get_nb_clauses() << "\n";
}
void run_hill_climb_ban(int argc, char *args[]) {
    std::shared_ptr<Formule> f = std::shared_ptr<Formule>(new Formule(args[1]));
    
    Solution solution(f);
    solution.randomize();
    std::unique_ptr<Instance> temp = Argmax::hill_climb_tab(solution.clone(), solution.nb_args() / 4, 2048);
    Solution result = *dynamic_cast<Solution*>(temp.get());

    std::cout << "max score: " << result << " : " << int(result.score()) << " out of " << f->get_nb_clauses() << "\n";
}
void run_simple_evo(int argc, char *args[]) {
    std::shared_ptr<Formule> f = std::shared_ptr<Formule>(new Formule(args[1]));
    
    Solution solution(f);
    auto p = Argmax::simple_evolution_parameters();
    p.mutation_probability = 0.01f;
    p.generation_count = 2048;

    std::unique_ptr<Instance> temp = Argmax::simple_evolution(
        [solution]() -> std::unique_ptr<Instance> { return solution.randomize_clone(); },
        p
    );
    Solution result = *dynamic_cast<Solution*>(temp.get());

    std::cout << "max score: " << result << " : " << int(result.score()) << " out of " << f->get_nb_clauses() << "\n";
}
void run_evo(int argc, char *args[]) {
    std::shared_ptr<Formule> f = std::shared_ptr<Formule>(new Formule(args[1]));

    Solution solution(f);
    auto p = Argmax::evolution_parameters();
    p.mutation_probability = 0.01f;
    p.generation_count = 256;

    std::unique_ptr<Instance> temp = Argmax::evolution(
        [solution]() -> std::unique_ptr<Instance> { return solution.randomize_clone(); },
        p,
        true
    );
    Solution result = *dynamic_cast<Solution*>(temp.get());

    std::cout << "max score: " << result << " : " << int(result.score()) << " out of " << f->get_nb_clauses() << "\n";
}

void run_hill_climb_fa(int argc, char *args[]) {
    std::shared_ptr<Deck> deck = std::make_shared<Deck>(args[1], args[2]);
    
    Hand h(deck);
    
    Hand best = h;
    unsigned int max = 0;
    int percent = 0;
    int nb_iter = 512;
    
    for (int i = 0; i <= nb_iter; i++)
    {
        h.randomize();
        std::unique_ptr<Instance> temp = Argmax::hill_climb(h.clone());
        Hand result = *dynamic_cast<Hand*>(temp.get());
    
        unsigned int count = result.score();
        if (count > max) {
            max = count;
            best = result;
        }

        int p = 100 * (float)i / nb_iter;
        if (p != percent) {
            percent = p;
            std::string t = "\rprogress: " + std::to_string(p);
            if (p < 10) t += " ";
            t += "% [";
            for (int i = 0; i < p / 10; i++) t += "=";//"▆";
            for (int i = p / 10; i < 10; i++) t += " ";
            
            std::cout << t + "]          ";
        }
    }

    std::cout << "max score: " << int(best.score()) << " points with hand\n";
    best.pretty_cout(std::cout);
}
void run_hill_climb_ban_fa(int argc, char *args[]) {
    std::shared_ptr<Deck> deck = std::make_shared<Deck>(args[1], args[2]);
    
    Hand h(deck);
    h.randomize();
    std::unique_ptr<Instance> temp = Argmax::hill_climb_tab(h.clone(), 3, 2048);
    Hand result = *dynamic_cast<Hand*>(temp.get());

    std::cout << "max score: " << int(result.score()) << " points with hand\n";
    result.pretty_cout(std::cout);
}
void run_simple_evo_fa(int argc, char *args[]) {
    std::shared_ptr<Deck> deck = std::make_shared<Deck>(args[1], args[2]);
    
    Hand h(deck);
    auto p = Argmax::simple_evolution_parameters();
    p.mutation_probability = 0.1f;
    p.generation_count = 256;

    std::unique_ptr<Instance> temp = Argmax::simple_evolution(
        [h]() -> std::unique_ptr<Instance> { return h.randomize_clone(); },
        p,
        true
    );
    Hand result = *dynamic_cast<Hand*>(temp.get());

    std::cout << "max score: " << int(result.score()) << " points with hand\n";
    result.pretty_cout(std::cout);
}
void run_evo_fa(int argc, char *args[]) {
    std::shared_ptr<Deck> deck = std::make_shared<Deck>(args[1], args[2]);
    
    Hand h(deck);
    auto p = Argmax::evolution_parameters();
    p.mutation_probability = 0.1f;
    p.generation_count = 256;

    std::unique_ptr<Instance> temp = Argmax::evolution(
        [h]() -> std::unique_ptr<Instance> { return h.randomize_clone(); },
        p,
        true
    );
    Hand result = *dynamic_cast<Hand*>(temp.get());

    std::cout << "max score: " << int(result.score()) << " points with hand\n";
    result.pretty_cout(std::cout);
}

int main(int argc, char *args[]) {
    system("chcp 65001");
    if (argc < 2) path_message();
    if (argc < 3) algo_message();

    srand(time(NULL));

    std::string arg2 = args[2];
    if (arg2 == "-hc") run_hill_climb(argc, args);
    else if (arg2 == "-hc_ban") run_hill_climb_ban(argc, args);
    else if (arg2 == "-evo_simple") run_simple_evo(argc, args);
    else if (arg2 == "-evo") run_evo(argc, args);

    else if (argc >= 4 && (std::string)args[3] == "-fa") {
        if (argc < 5) algo_message();

        std::string arg4 = args[4];
        if (arg4 == "-hc") run_hill_climb_fa(argc, args);
        else if (arg4 == "-hc_ban") run_hill_climb_ban_fa(argc, args);
        else if (arg4 == "-evo_simple") run_simple_evo_fa(argc, args);
        else if (arg4 == "-evo") run_evo_fa(argc, args);
    }
    else algo_message();

    return 0;
}