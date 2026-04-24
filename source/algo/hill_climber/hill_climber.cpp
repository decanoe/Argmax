#include "hill_climber.h"

using namespace LocalSearch;

/* #region ======================================= HillClimber::Selection_Criterion ======================================= */
unsigned int HillClimber::Selection_Criterion::get_test_index(unsigned int default_index, unsigned int iteration_index, unsigned int nb_args) { return default_index; }

class HC_Best_Criterion: public HillClimber::Selection_Criterion {
public:
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score) const override {
        return (tested_score > init_score) && (first_keep || (tested_score > iteration_best_score));
    }
    bool stop_at_first_improve() const override { return false; }
protected:
};
class HC_Least_Criterion: public HillClimber::Selection_Criterion {
public:
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score) const override {
        return (tested_score > init_score) && (first_keep || (tested_score < iteration_best_score));
    }
    bool stop_at_first_improve() const override { return false; }
protected:
};
class HC_Random_Criterion: public HillClimber::Selection_Criterion {
public:
    HC_Random_Criterion(): visited_indices() {}

    unsigned int get_test_index(unsigned int default_index, unsigned int iteration_index, unsigned int nb_args) override {
        if (default_index == 0) this->visited_indices.clear();
        unsigned int index = RandomUtils::get_index(nb_args, visited_indices, *this->random_generator);
        visited_indices.insert(index);

        return index;
    }
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score) const override {
        return (tested_score > init_score);
    }
    bool stop_at_first_improve() const override { return true; }
protected:
    std::set<unsigned int> visited_indices;
};
class HC_First_Criterion: public HillClimber::Selection_Criterion {
public:
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score) const override {
        return (tested_score > init_score);
    }
    bool stop_at_first_improve() const override { return true; }
protected:
};
class HC_Cycle_Criterion: public HillClimber::Selection_Criterion {
public:
    unsigned int get_test_index(unsigned int default_index, unsigned int iteration_index, unsigned int nb_args) override {
        return (default_index + iteration_index) % nb_args;
    }
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score) const override {
        return (tested_score > init_score);
    }
    bool stop_at_first_improve() const override { return true; }
protected:
};

std::shared_ptr<HillClimber::Selection_Criterion> HillClimber::Selection_Criterion::from_file_data(const FileData& file_data) {
    std::string string = file_data.get_string("selection_criterion");
    if (string == "random") return std::make_shared<HC_Random_Criterion>();
    else if (string == "first") return std::make_shared<HC_First_Criterion>();
    else if (string == "cycle") return std::make_shared<HC_Cycle_Criterion>();
    else if (string == "least") return std::make_shared<HC_Least_Criterion>();
    else if (string == "best") return std::make_shared<HC_Best_Criterion>();
    
    return std::make_shared<HC_First_Criterion>(); // default
}
/* #endregion */

/* #region ======================================= HillClimber ======================================= */
HillClimber::HillClimber(std::shared_ptr<Selection_Criterion> criterion): LocalSearchAlgo(), criterion(criterion) {}
LocalSearchAlgo* HillClimber::set_seed(std::shared_ptr<std::mt19937> random_generator) {
    criterion->set_seed(random_generator);
    return LocalSearchAlgo::set_seed(random_generator);
}

void HillClimber::improve(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) const {
    float score = instance->score();
    budget++;
    unsigned int better_neighbors = count_better_neighbors(instance);
    unsigned int better_neighbors_after = better_neighbors;
    output_iteration_ends_data(budget, better_neighbors, score);
    
    unsigned int iteration_count = 0;
    while (!budget.out_of_budget())
    {
        unsigned int iteration_best_move = -1U;
        float iteration_best_score = 0;

        // iterate through mutations
        for (size_t i = 0; i < instance->nb_args() && !budget.out_of_budget(); i++)
        {
            unsigned int index = criterion->get_test_index(i, iteration_count, instance->nb_args());

            instance->mutate_arg(index);
            budget++;
            if (criterion->do_keep(instance->score(), score, iteration_best_move == -1U, iteration_best_score)) {
                iteration_best_move = index;
                iteration_best_score = instance->score();
                if (criterion->stop_at_first_improve()) { instance->revert_last_mutation(); break; }
            }
            instance->revert_last_mutation();
        }

        // apply best mutation
        if (iteration_best_move != -1U) {
            float old_score = instance->score();
            instance->mutate_arg(iteration_best_move);
            better_neighbors_after = count_better_neighbors(instance);
            output_iteration_data(budget, better_neighbors, better_neighbors_after, old_score, iteration_best_score, 1);
            better_neighbors = better_neighbors_after;
            score = iteration_best_score;
        }
        else {
            output_iteration_ends_data(budget, better_neighbors, score);
            break;
        }
        iteration_count++;
    }
}
/* #endregion */

