#include "source/argmax.h"
#include "sat_specific/formule.h"
#include "sat_specific/solution.h"
#include <time.h>
// chcp 65001
#define __max(a,b) (((a) > (b)) ? (a) : (b))

int main(int argc, char *args[]) {
    srand(time(NULL));

    if (argc < 2) {
        std::cerr << "\033[1;31mYou need to put a path to a cnf (or txt) file\n\033[0m";
        exit(1);
    }
    std::shared_ptr<Formule> f = std::shared_ptr<Formule>(new Formule(args[1]));
    std::cout << "file loaded\n";
    
    Solution solution(f);
    
    std::string best = "no best";
    unsigned int max = 0;
    // int percent = 0;
    int nb_iter = 2024;
    
    
    solution.randomize();
    std::unique_ptr<Instance> temp = hill_climb_tab(solution.clone(), solution.nb_args() / 4, nb_iter);
    Solution result = *dynamic_cast<Solution*>(temp.get());
    max = result.score();
    /*
    for (int i = 0; i <= nb_iter; i++)
    {
        solution.randomize();
        std::unique_ptr<Instance> temp = hill_climb(solution.clone());
        Solution result = *dynamic_cast<Solution*>(temp.get());
    
        unsigned int count = result.score();
        if (count > max) {
            max = count;
            best = result.to_string(true);
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
    */
    std::cout << "\nmax score: " << result << " : " << max << " out of " << f->get_nb_clauses() << "\n";
    return 0;
}