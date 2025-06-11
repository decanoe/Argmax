#include "local_search.h"
#include <limits>

auto cmp = [](std::pair<unsigned int, float> a, std::pair<unsigned int, float> b) {
    if (a.second != b.second) return a.second > b.second;
    else return a.first < b.first;
};

unsigned int count_better_neighbors(ReversibleInstance* instance) {
    unsigned int result = 0;
    float score = instance->score();
    for (size_t i = 0; i < instance->nb_args(); i++) {
        instance->mutate_arg(i);
        if (instance->score() > score) result++;
        instance->revert_last_mutation();
    }
    return result;
}
unsigned int count_better_neighbors(std::unique_ptr<Instance>& instance) {
    unsigned int result = 0;
    float score = instance->score();
    for (size_t i = 0; i < instance->nb_args(); i++) {
        std::unique_ptr<Instance> temp = instance->clone();
        temp->mutate_arg(i);
        if (temp->score() > score) result++;
    }
    return result;
}

unsigned int hill_climb_random                          (ReversibleInstance* instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
    while (used_budget < budget)
    {
        bool better = false;

        std::set<unsigned int> visited = std::set<unsigned int>();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            unsigned int index = RandomUtils::get_index(instance->nb_args(), visited);
            visited.insert(index);
            instance->mutate_arg(index);
            used_budget++;
            if (instance->score() > score) {
                score = instance->score();
                better = true;
                better_neighbors = count_better_neighbors(instance);
                if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t1\n"; }
                break;
            }
            instance->revert_last_mutation();
        }
        if (!better) {
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
            break;
        }
    }

    return used_budget;
}
unsigned int LocalSearch::hill_climb_random             (std::unique_ptr<Instance>& instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (ReversibleInstance *r_instance = dynamic_cast<ReversibleInstance *>(instance.get())) return hill_climb_random(r_instance, budget, initial_budget, out, add_header);
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
    while (used_budget < budget)
    {
        bool better = false;
        
        std::set<unsigned int> visited = std::set<unsigned int>();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            unsigned int index = RandomUtils::get_index(instance->nb_args(), visited);
            visited.insert(index);
            std::unique_ptr<Instance> temp = instance->clone();
            temp->mutate_arg(index);
            used_budget++;
            if (temp->score() > score) {
                score = temp->score();
                instance = std::move(temp);
                better = true;
                better_neighbors = count_better_neighbors(instance);
                if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t1\n"; }
                break;
            }
        }
        if (!better) {
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
            break;
        }
    }
    return used_budget;
}
unsigned int hill_climb_first                           (ReversibleInstance* instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
    while (used_budget < budget)
    {
        bool better = false;
        
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            if (instance->score() > score) {
                score = instance->score();
                better = true;
                better_neighbors = count_better_neighbors(instance);
                if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t1\n"; }
                break;
            }
            instance->revert_last_mutation();
        }
        if (!better) {
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
            break;
        }
    }
    return used_budget;
}
unsigned int LocalSearch::hill_climb_first              (std::unique_ptr<Instance>& instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (ReversibleInstance *r_instance = dynamic_cast<ReversibleInstance *>(instance.get())) return hill_climb_first(r_instance, budget, initial_budget, out, add_header);
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
    while (used_budget < budget)
    {
        bool better = false;
        
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            std::unique_ptr<Instance> temp = instance->clone();
            temp->mutate_arg(i);
            used_budget++;
            if (temp->score() > score) {
                score = temp->score();
                instance = std::move(temp);
                better = true;
                better_neighbors = count_better_neighbors(instance);
                if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t1\n"; }
                break;
            }
        }
        if (!better) {
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
            break;
        }
    }
    return used_budget;
}
unsigned int hill_climb_cycle                           (ReversibleInstance* instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
    unsigned int loop = 0;
    while (used_budget < budget)
    {
        bool better = false;
        
        for (unsigned int j = 0; j < instance->nb_args() && used_budget < budget; j++)
        {
            unsigned int i = (j + loop) % instance->nb_args();
            instance->mutate_arg(i);
            used_budget++;
            if (instance->score() > score) {
                score = instance->score();
                better = true;
                better_neighbors = count_better_neighbors(instance);
                if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t1\n"; }
                break;
            }
            instance->revert_last_mutation();
        }
        if (!better) {
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
            break;
        }
        loop++;
    }
    return used_budget;
}
unsigned int LocalSearch::hill_climb_cycle              (std::unique_ptr<Instance>& instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (ReversibleInstance *r_instance = dynamic_cast<ReversibleInstance *>(instance.get())) return hill_climb_cycle(r_instance, budget, initial_budget, out, add_header);
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
    unsigned int loop = 0;
    while (used_budget < budget)
    {
        bool better = false;
        
        for (unsigned int j = 0; j < instance->nb_args() && used_budget < budget; j++)
        {
            unsigned int i = (j + loop) % instance->nb_args();
            std::unique_ptr<Instance> temp = instance->clone();
            temp->mutate_arg(i);
            used_budget++;
            if (temp->score() > score) {
                score = temp->score();
                instance = std::move(temp);
                better = true;
                better_neighbors = count_better_neighbors(instance);
                if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t1\n"; }
                break;
            }
        }
        if (!better) {
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }
            break;
        }
        loop++;
    }
    return used_budget;
}
unsigned int hill_climb_best                            (ReversibleInstance* instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }

    unsigned int best = -1U;
    float best_score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << best_score << "\t" << better_neighbors << "\t0\n"; }
    while (used_budget < budget)
    {
        best = -1U;
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            if (instance->score() > best_score) {
                best = i;
                best_score = instance->score();
            }
            instance->revert_last_mutation();
        }
        if (best == -1U) {
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << best_score << "\t" << better_neighbors << "\t0\n"; }
            break;
        }
        instance->mutate_arg(best);
        better_neighbors = count_better_neighbors(instance);
        if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << best_score << "\t" << better_neighbors << "\t1\n"; }
    }
    return used_budget;
}
unsigned int LocalSearch::hill_climb_best               (std::unique_ptr<Instance>& instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (ReversibleInstance *r_instance = dynamic_cast<ReversibleInstance *>(instance.get())) return hill_climb_best(r_instance, budget, initial_budget, out, add_header);
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }

    float best_score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << best_score << "\t" << better_neighbors << "\t0\n"; }
    while (used_budget < budget)
    {
        std::unique_ptr<Instance> best = nullptr;
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            std::unique_ptr<Instance> temp = instance->clone();
            temp->mutate_arg(i);
            used_budget++;
            if (temp->score() > best_score) {
                best = std::move(temp);
                best_score = temp->score();
            }
        }
        if (best == nullptr) {
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << best_score << "\t" << better_neighbors << "\t0\n"; }
            break;
        }
        instance = std::move(best);
        better_neighbors = count_better_neighbors(instance);
        if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << best_score << "\t" << better_neighbors << "\t1\n"; }
    }
    return used_budget;
}
unsigned int hill_climb_least                           (ReversibleInstance* instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }

    unsigned int best = -1U;
    float best_score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << best_score << "\t" << better_neighbors << "\t0\n"; }
    while (used_budget < budget)
    {
        best = -1U;
        float old_best = best_score;
        best_score = std::numeric_limits<float>::max();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            if (instance->score() > old_best && instance->score() < best_score) {
                best = i;
                best_score = instance->score();
            }
            instance->revert_last_mutation();
        }
        if (best == -1U) {
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << old_best << "\t" << better_neighbors << "\t0\n"; }
            break;
        }
        instance->mutate_arg(best);
        better_neighbors = count_better_neighbors(instance);
        if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << best_score << "\t" << better_neighbors << "\t1\n"; }
    }
    return used_budget;
}
unsigned int LocalSearch::hill_climb_least              (std::unique_ptr<Instance>& instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (ReversibleInstance *r_instance = dynamic_cast<ReversibleInstance *>(instance.get())) return hill_climb_least(r_instance, budget, initial_budget, out, add_header);
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }

    float best_score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << best_score << "\t" << better_neighbors << "\t0\n"; }
    while (used_budget < budget)
    {
        std::unique_ptr<Instance> best = nullptr;
        float old_best = best_score;
        best_score = std::numeric_limits<float>::max();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            std::unique_ptr<Instance> temp = instance->clone();
            temp->mutate_arg(i);
            used_budget++;
            if (temp->score() > old_best && temp->score() < best_score) {
                best = std::move(temp);
                best_score = temp->score();
            }
        }
        if (best == nullptr) {
            if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << old_best << "\t" << better_neighbors << "\t0\n"; }
            break;
        }
        instance = std::move(best);
        better_neighbors = count_better_neighbors(instance);
        if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << best_score << "\t" << better_neighbors << "\t1\n"; }
    }
    return used_budget;
}

unsigned int LocalSearch::hill_climb_greedy_first       (std::unique_ptr<Instance>& instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (dynamic_cast<ReversibleInstance *>(instance.get()) == nullptr) {
        std::cerr << "\033[1;31mERROR: cannot do a hill_climb_greedy on non reversible instance\033[0m\n";
        exit(1);
    }
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }

    std::set<std::pair<unsigned int, float>, decltype(cmp)> sorted_list(cmp);
    while (used_budget < budget)
    {
        sorted_list.clear();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            if (instance->score() > score) sorted_list.insert({i, instance->score()});
            instance->mutate_arg(i);
        }
        for (std::pair<unsigned int, float> pair : sorted_list) instance->mutate_arg(pair.first);
        unsigned int count = sorted_list.size();
        for (auto pair = sorted_list.rbegin(); pair != sorted_list.rend(); pair++) {
            used_budget++;
            if (instance->score() > score) {
                score = instance->score();
                break;
            }
            else {
                count--;
                instance->mutate_arg(pair->first);
            }
        }
        better_neighbors = count_better_neighbors(instance);
        if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t" << count << "\n"; }
        if (count == 0) return used_budget;
    }
    return used_budget;
}
unsigned int LocalSearch::hill_climb_greedy_best        (std::unique_ptr<Instance>& instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (dynamic_cast<ReversibleInstance *>(instance.get()) == nullptr) {
        std::cerr << "\033[1;31mERROR: cannot do a hill_climb_greedy on non reversible instance\033[0m\n";
        exit(1);
    }
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }

    std::set<std::pair<unsigned int, float>, decltype(cmp)> sorted_list(cmp);
    while (used_budget < budget)
    {
        sorted_list.clear();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            if (instance->score() > score) sorted_list.insert({i, instance->score()});
            instance->mutate_arg(i);
        }
        for (std::pair<unsigned int, float> pair : sorted_list) instance->mutate_arg(pair.first);
        unsigned int count = sorted_list.size();
        unsigned int best_count = 0;
        for (auto pair = sorted_list.rbegin(); pair != sorted_list.rend(); pair++) {
            used_budget++;
            if (instance->score() > score) {
                score = instance->score();
                best_count = count;
            }
            count--;
            instance->mutate_arg(pair->first);
        }
        count = best_count;
        for (std::pair<unsigned int, float> pair : sorted_list) {
            if (count == 0) break;
            count--;
            instance->mutate_arg(pair.first);
        }
        better_neighbors = count_better_neighbors(instance);
        if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t" << best_count << "\n"; }
        if (best_count == 0) return used_budget;
    }
    return used_budget;
}
unsigned int LocalSearch::hill_climb_greedy_least       (std::unique_ptr<Instance>& instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (dynamic_cast<ReversibleInstance *>(instance.get()) == nullptr) {
        std::cerr << "\033[1;31mERROR: cannot do a hill_climb_greedy on non reversible instance\033[0m\n";
        exit(1);
    }
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }

    std::set<std::pair<unsigned int, float>, decltype(cmp)> sorted_list(cmp);
    while (used_budget < budget)
    {
        sorted_list.clear();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            if (instance->score() > score) sorted_list.insert({i, instance->score()});
            instance->mutate_arg(i);
        }
        for (std::pair<unsigned int, float> pair : sorted_list) instance->mutate_arg(pair.first);
        unsigned int count = sorted_list.size();
        unsigned int best_count = 0;
        float old_score = score;
        score = std::numeric_limits<float>::max();
        for (auto pair = sorted_list.rbegin(); pair != sorted_list.rend(); pair++) {
            used_budget++;
            if (instance->score() > old_score && instance->score() < score) {
                score = instance->score();
                best_count = count;
            }
            count--;
            instance->mutate_arg(pair->first);
        }
        count = best_count;
        for (std::pair<unsigned int, float> pair : sorted_list) {
            if (count == 0) break;
            count--;
            instance->mutate_arg(pair.first);
        }
        better_neighbors = count_better_neighbors(instance);
        if (best_count == 0) score = old_score;
        if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t" << best_count << "\n"; }
        if (best_count == 0) return used_budget;
    }
    return used_budget;
}

unsigned int LocalSearch::hill_climb_greedy_all_first   (std::unique_ptr<Instance>& instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (dynamic_cast<ReversibleInstance *>(instance.get()) == nullptr) {
        std::cerr << "\033[1;31mERROR: cannot do a hill_climb_greedy on non reversible instance\033[0m\n";
        exit(1);
    }
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }

    std::set<std::pair<unsigned int, float>, decltype(cmp)> sorted_list(cmp);
    while (used_budget < budget)
    {
        sorted_list.clear();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            sorted_list.insert({i, instance->score()});
            instance->mutate_arg(i);
        }
        for (std::pair<unsigned int, float> pair : sorted_list) instance->mutate_arg(pair.first);
        unsigned int count = sorted_list.size();
        for (auto pair = sorted_list.rbegin(); pair != sorted_list.rend(); pair++) {
            used_budget++;
            if (instance->score() > score) {
                score = instance->score();
                break;
            }
            else {
                count--;
                instance->mutate_arg(pair->first);
            }
        }
        better_neighbors = count_better_neighbors(instance);
        if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t" << count << "\n"; }
        if (count == 0) return used_budget;
    }
    return used_budget;
}
unsigned int LocalSearch::hill_climb_greedy_all_best    (std::unique_ptr<Instance>& instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (dynamic_cast<ReversibleInstance *>(instance.get()) == nullptr) {
        std::cerr << "\033[1;31mERROR: cannot do a hill_climb_greedy on non reversible instance\033[0m\n";
        exit(1);
    }
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }

    std::set<std::pair<unsigned int, float>, decltype(cmp)> sorted_list(cmp);
    while (used_budget < budget)
    {
        sorted_list.clear();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            sorted_list.insert({i, instance->score()});
            instance->mutate_arg(i);
        }
        for (std::pair<unsigned int, float> pair : sorted_list) instance->mutate_arg(pair.first);
        unsigned int count = sorted_list.size();
        unsigned int best_count = 0;
        for (auto pair = sorted_list.rbegin(); pair != sorted_list.rend(); pair++) {
            used_budget++;
            if (instance->score() > score) {
                score = instance->score();
                best_count = count;
            }
            count--;
            instance->mutate_arg(pair->first);
        }
        count = best_count;
        for (std::pair<unsigned int, float> pair : sorted_list) {
            if (count == 0) break;
            count--;
            instance->mutate_arg(pair.first);
        }
        better_neighbors = count_better_neighbors(instance);
        if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t" << best_count << "\n"; }
        if (best_count == 0) return used_budget;
    }
    return used_budget;
}
unsigned int LocalSearch::hill_climb_greedy_all_least   (std::unique_ptr<Instance>& instance, unsigned int budget, unsigned int initial_budget, std::ofstream* out, bool add_header) {
    if (dynamic_cast<ReversibleInstance *>(instance.get()) == nullptr) {
        std::cerr << "\033[1;31mERROR: cannot do a hill_climb_greedy on non reversible instance\033[0m\n";
        exit(1);
    }
    if (out && add_header) { *out << "budget\tin_run_budget\tfitness\tnb_better_neighbors\tsize_of_the_jump" << "\n"; }
    
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t0\n"; }

    std::set<std::pair<unsigned int, float>, decltype(cmp)> sorted_list(cmp);
    while (used_budget < budget)
    {
        sorted_list.clear();
        for (size_t i = 0; i < instance->nb_args() && used_budget < budget; i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            sorted_list.insert({i, instance->score()});
            instance->mutate_arg(i);
        }
        for (std::pair<unsigned int, float> pair : sorted_list) instance->mutate_arg(pair.first);
        unsigned int count = sorted_list.size();
        unsigned int best_count = 0;
        float old_score = score;
        score = std::numeric_limits<float>::max();
        for (auto pair = sorted_list.rbegin(); pair != sorted_list.rend(); pair++) {
            used_budget++;
            if (instance->score() > old_score && instance->score() < score) {
                score = instance->score();
                best_count = count;
            }
            count--;
            instance->mutate_arg(pair->first);
        }
        count = best_count;
        for (std::pair<unsigned int, float> pair : sorted_list) {
            if (count == 0) break;
            count--;
            instance->mutate_arg(pair.first);
        }
        better_neighbors = count_better_neighbors(instance);
        if (best_count == 0) score = old_score;
        if (out) { *out << used_budget << "\t" << (used_budget  - initial_budget) << "\t" << score << "\t" << better_neighbors << "\t" << best_count << "\n"; }
        if (best_count == 0) return used_budget;
    }
    return used_budget;
}
