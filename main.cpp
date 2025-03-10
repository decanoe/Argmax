#include "source/argmax.h"
#include "sat_specific/formule.h"
#include "sat_specific/solution.h"
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
\t-evo_simple \tto use the simple evolution algorithm\n\033[0m";
    exit(2);
}

void run_hill_climb(int argc, char *args[]) {
    std::shared_ptr<Formule> f = std::shared_ptr<Formule>(new Formule(args[1]));
    std::cout << "file loaded\n";
    
    Solution solution(f);
    
    Solution best = solution;
    unsigned int max = 0;
    int percent = 0;
    int nb_iter = 2028;
    
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

    std::cout << "\nmax score: " << result << " : " << int(result.score()) << " out of " << f->get_nb_clauses() << "\n";
}
void run_simple_evo(int argc, char *args[]) {
    std::shared_ptr<Formule> f = std::shared_ptr<Formule>(new Formule(args[1]));
    
    Solution solution(f);
    
    std::unique_ptr<Instance> temp = Argmax::simple_evolution(
        [solution]() -> std::unique_ptr<Instance> { return solution.randomize_clone(); },
        Argmax::simple_evolution_parameters()
    );
    Solution result = *dynamic_cast<Solution*>(temp.get());

    std::cout << "\nmax score: " << result << " : " << int(result.score()) << " out of " << f->get_nb_clauses() << "\n";
}

int main(int argc, char *args[]) {
    if (argc < 2) path_message();
    if (argc < 3) algo_message();

    srand(time(NULL));

    std::string arg2 = args[2];
    if (arg2 == "-hc") run_hill_climb(argc, args);
    else if (arg2 == "-hc_ban") run_hill_climb_ban(argc, args);
    else if (arg2 == "-evo_simple") run_simple_evo(argc, args);
    else algo_message();

    return 0;
}