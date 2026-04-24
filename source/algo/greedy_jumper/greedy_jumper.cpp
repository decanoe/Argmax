#include "greedy_jumper.h"

using namespace LocalSearch;

/* #region ======================================= GreedyJumper::Selection_Criterion ======================================= */
class GJ_Best_Criterion: public GreedyJumper::Selection_Criterion {
public:
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score) const override {
        return (tested_score > init_score) && (first_keep || (tested_score > iteration_best_score));
    }
    bool stop_at_first_improve() const override {
        return false;
    }
protected:
};
class GJ_Least_Criterion: public GreedyJumper::Selection_Criterion {
public:
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score) const override {
        return (tested_score > init_score) && (first_keep || (tested_score < iteration_best_score));
    }
    bool stop_at_first_improve() const override {
        return false;
    }
protected:
};
class GJ_First_Criterion: public GreedyJumper::Selection_Criterion {
public:
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score) const override {
        return (tested_score > init_score);
    }
    bool stop_at_first_improve() const override {
        return true;
    }
protected:
};
class GJ_Random_Criterion: public GJ_First_Criterion {
public:
    unsigned int chose_jump(const GreedyJumper::TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float& instance_score, BudgetHelper& budget) const override {
        std::vector<unsigned int> flips = std::vector<unsigned int>();
        for (std::pair<unsigned int, float> pair : trajectory) flips.push_back(pair.first);
        

        std::set<unsigned int> tested_jumps = std::set<unsigned int>();
        while (tested_jumps.size() < flips.size())
        {
            unsigned int jump_size = RandomUtils::get_index(flips.size(), tested_jumps, *this->random_generator);
            
            for (unsigned int i = 0; i < jump_size; i++) instance->mutate_arg(flips[i]);
            
            budget++;
            if (instance->score() > instance_score) {
                instance_score = instance->score();
                return jump_size;
            }

            for (unsigned int i = 0; i < jump_size; i++) instance->mutate_arg(flips[i]);
        }

        return 0;
    }
protected:
};

unsigned int GreedyJumper::Selection_Criterion::chose_jump(const TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float& instance_score, BudgetHelper& budget) const {
    // apply all mutations in trajectory
    for (std::pair<unsigned int, float> pair : trajectory) instance->mutate_arg(pair.first);
    
    // compare jumps of the trajectory
    unsigned int count = trajectory.size();
    unsigned int best_count = 0;
    float best_score = instance_score;
    for (auto pair = trajectory.rbegin(); pair != trajectory.rend(); pair++) {
        budget++;
        if (this->do_keep(instance->score(), instance_score, best_count == 0, best_score)) {
            best_score = instance->score();
            best_count = count;
            if (this->stop_at_first_improve()) break;
        }
        count--;
        instance->mutate_arg(pair->first);
    }

    // apply best jump found
    instance_score = best_score;
    if (!this->stop_at_first_improve()) {
        count = best_count;
        for (std::pair<unsigned int, float> pair : trajectory) {
            if (count == 0) break;
            count--;
            instance->mutate_arg(pair.first);
        }
    }

    return best_count;
}
std::shared_ptr<GreedyJumper::Selection_Criterion> GreedyJumper::Selection_Criterion::from_file_data(const FileData& file_data) {
    std::string string = file_data.get_string("selection_criterion");
    if (string == "first") return std::make_shared<GJ_First_Criterion>();
    else if (string == "best") return std::make_shared<GJ_Best_Criterion>();
    else if (string == "least") return std::make_shared<GJ_Least_Criterion>();
    else if (string == "random") return std::make_shared<GJ_Random_Criterion>();
    
    return std::make_shared<GJ_First_Criterion>(); // default
}
/* #endregion */
/* #region ======================================= GreedyJumper::Neighborhood_Scope ======================================= */
class GJ_Full_Scope: public GreedyJumper::Neighborhood_Scope {
public:
    void create_trajectory(GreedyJumper::TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget, std::function<bool(unsigned int)> is_valid) const override {
        // create trajectory
        trajectory.clear();
        for (size_t i = 0; i < instance->nb_args() && !budget.out_of_budget(); i++)
        {
            if (!is_valid(i)) continue;
            instance->mutate_arg(i);
            budget++;
            trajectory.insert({i, instance->score()});
            instance->revert_last_mutation();
        }
    }
protected:
};
class GJ_Improve_Scope: public GreedyJumper::Neighborhood_Scope {
public:
    void create_trajectory(GreedyJumper::TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget, std::function<bool(unsigned int)> is_valid) const override {
        // create trajectory
        trajectory.clear();
        for (size_t i = 0; i < instance->nb_args() && !budget.out_of_budget(); i++)
        {
            if (!is_valid(i)) continue;
            instance->mutate_arg(i);
            budget++;
            if (instance->score() > score) trajectory.insert({i, instance->score()});
            instance->revert_last_mutation();
        }
    }
protected:
};
class GJ_Fixed_Scope: public GreedyJumper::Neighborhood_Scope {
public:
    GJ_Fixed_Scope(float keep_factor): keep_factor(keep_factor) {}

    void create_trajectory(GreedyJumper::TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget, std::function<bool(unsigned int)> is_valid) const override {
        // create trajectory
        trajectory.clear();
        for (size_t i = 0; i < instance->nb_args() && !budget.out_of_budget(); i++)
        {
            if (!is_valid(i)) continue;
            instance->mutate_arg(i);
            budget++;
            trajectory.insert({i, instance->score()});
            instance->revert_last_mutation();
        }
        while (trajectory.size() > instance->nb_args() * keep_factor) trajectory.erase(std::next(trajectory.rbegin()).base());
    }
protected:
    float keep_factor = .5;
};

void GreedyJumper::Neighborhood_Scope::create_trajectory(GreedyJumper::TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget) const {
    return create_trajectory(trajectory, instance, score, budget, [](unsigned int) { return true; });
}
std::shared_ptr<GreedyJumper::Neighborhood_Scope> GreedyJumper::Neighborhood_Scope::from_file_data(const FileData& file_data) {
    std::string string = file_data.get_string("neighborhood_scope");
    if (string == "improve") return std::make_shared<GJ_Improve_Scope>();
    else if (string == "all") return std::make_shared<GJ_Full_Scope>();
    else if (string == "half") return std::make_shared<GJ_Fixed_Scope>(.5);
    else if (string == "fixed") return std::make_shared<GJ_Fixed_Scope>(file_data.get_float("max_flip_factor"));
    
    return std::make_shared<GJ_Full_Scope>(); // default
}
/* #endregion */

/* #region ======================================= GreedyJumper ======================================= */
GreedyJumper::GreedyJumper(std::shared_ptr<Selection_Criterion> criterion, std::shared_ptr<Neighborhood_Scope> scope): criterion(criterion), scope(scope) {}
LocalSearchAlgo* GreedyJumper::set_seed(std::shared_ptr<std::mt19937> random_generator) {
    criterion->set_seed(random_generator);
    scope->set_seed(random_generator);
    return LocalSearchAlgo::set_seed(random_generator);
}

bool GreedyJumper::cmp(std::pair<unsigned int, float> a, std::pair<unsigned int, float> b) {
    if (a.second != b.second) return a.second > b.second;
    else return a.first < b.first;
};

void GreedyJumper::improve(std::unique_ptr<ReversibleInstance>& instance, BudgetHelper& budget) const {
    float score = instance->score();
    budget++;
    unsigned int better_neighbors = count_better_neighbors(instance);
    unsigned int better_neighbors_after = better_neighbors;
    output_iteration_ends_data(budget, better_neighbors, score);

    GreedyJumper::TrajectorySet trajectory(cmp);
    while (!budget.out_of_budget())
    {
        // create trajectory
        this->scope->create_trajectory(trajectory, instance, score, budget);

        // chose jump
        float old_score = score;
        unsigned int jump_size = this->criterion->chose_jump(trajectory, instance, score, budget);
        
        better_neighbors_after = count_better_neighbors(instance);
        output_iteration_data(budget, better_neighbors, better_neighbors_after, old_score, score, jump_size);
        better_neighbors = better_neighbors_after;
        if (jump_size == 0) break;
    }
}
/* #endregion */
