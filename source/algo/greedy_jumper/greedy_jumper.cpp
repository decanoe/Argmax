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
    unsigned int chose_jump(const GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float& instance_score, BudgetHelper& budget) const override {
        std::vector<unsigned int> flips = std::vector<unsigned int>();
        for (const BitFlip& bitflip : trajectory) flips.push_back(bitflip.index);
        

        std::set<unsigned int> tested_last_flip_index = std::set<unsigned int>();
        while (tested_last_flip_index.size() < flips.size())
        {
            unsigned int last_flip_index = RandomUtils::get_index(flips.size(), tested_last_flip_index, *this->random_generator);
            tested_last_flip_index.insert(last_flip_index);
            
            for (unsigned int i = 0; i <= last_flip_index; i++) instance->mutate_arg(flips[i]);
            
            budget++;
            if (instance->score() > instance_score) {
                instance_score = instance->score();
                return last_flip_index + 1;
            }

            for (unsigned int i = 0; i <= last_flip_index; i++) instance->mutate_arg(flips[i]);
        }

        return 0;
    }
protected:
};

unsigned int GreedyJumper::Selection_Criterion::chose_jump(const GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float& instance_score, BudgetHelper& budget) const {
    // apply all mutations in trajectory
    for (const BitFlip& bitflip : trajectory) instance->mutate_arg(bitflip.index);
    
    // compare jumps of the trajectory
    unsigned int count = trajectory.size();
    unsigned int best_count = 0;
    float best_score = instance_score;
    for (auto bitflip = trajectory.rbegin(); bitflip != trajectory.rend(); ++bitflip) {
        budget++;
        if (this->do_keep(instance->score(), instance_score, best_count == 0, best_score)) {
            best_score = instance->score();
            best_count = count;
            if (this->stop_at_first_improve()) break;
        }
        count--;
        instance->mutate_arg((*bitflip).index);
    }

    // apply best jump found
    instance_score = best_score;
    if (!this->stop_at_first_improve()) {
        count = best_count;
        for (const BitFlip& bitflip : trajectory) {
            if (count == 0) break;
            count--;
            instance->mutate_arg(bitflip.index);
        }
    }

    return best_count;
}
std::shared_ptr<GreedyJumper::Selection_Criterion> GreedyJumper::Selection_Criterion::from_file_data(const Parameters& parameters) {
    std::string string = parameters.get_string("selection_criterion");
    if (string == "first") return std::make_shared<GJ_First_Criterion>();
    else if (string == "best") return std::make_shared<GJ_Best_Criterion>();
    else if (string == "least") return std::make_shared<GJ_Least_Criterion>();
    else if (string == "random") return std::make_shared<GJ_Random_Criterion>();
    
    return std::make_shared<GJ_First_Criterion>(); // default
}
/* #endregion */
/* #region ======================================= GreedyJumper::Neighborhood_Scope ======================================= */
class GJ_Improve_Scope: public GreedyJumper::Neighborhood_Scope {
public:
    void create_trajectory(GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget, std::function<bool(unsigned int)> is_valid) const override {
        add_bit_flips(trajectory, instance, score, budget, is_valid);
        trajectory.finalize(trajectory.positive_size());
    }
protected:
};
class GJ_Fixed_Scope: public GreedyJumper::Neighborhood_Scope {
public:
    GJ_Fixed_Scope(float keep_factor): keep_factor(keep_factor) {}

    void create_trajectory(GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget, std::function<bool(unsigned int)> is_valid) const override {
        add_bit_flips(trajectory, instance, score, budget, is_valid);
        trajectory.finalize(instance->nb_args() * keep_factor);
    }
protected:
    float keep_factor = .5;
};
class GJ_Adaptative_Scope: public GreedyJumper::Neighborhood_Scope {
public:
    void create_trajectory(GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget, std::function<bool(unsigned int)> is_valid) const override {
        add_bit_flips(trajectory, instance, score, budget, is_valid);
        trajectory.finalize(std::max(trajectory.positive_size() * 2, 5U));
    }
protected:
};

void GreedyJumper::Neighborhood_Scope::create_trajectory(GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget) const {
    return create_trajectory(trajectory, instance, score, budget, [](unsigned int) { return true; });
}
void GreedyJumper::Neighborhood_Scope::add_bit_flips(GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget, std::function<bool(unsigned int)> is_valid) const {
    trajectory.clear();
    for (size_t i = 0; i < instance->nb_args() && !budget.out_of_budget(); i++)
    {
        if (!is_valid(i)) continue;
        instance->mutate_arg(i);
        budget++;

        if (instance->score() > score)
            trajectory.insert_positive_flip(BitFlip(i, instance->score()));
        else
            trajectory.insert_negative_flip(BitFlip(i, instance->score()));
        
        instance->revert_last_mutation();
    }
}

std::shared_ptr<GreedyJumper::Neighborhood_Scope> GreedyJumper::Neighborhood_Scope::from_file_data(const Parameters& parameters) {
    std::string string = parameters.get_string("neighborhood_scope");
    if (string == "improve") return std::make_shared<GJ_Improve_Scope>();
    else if (string == "all") return std::make_shared<GJ_Fixed_Scope>(1);
    else if (string == "half") return std::make_shared<GJ_Fixed_Scope>(.5);
    else if (string == "fixed") return std::make_shared<GJ_Fixed_Scope>(parameters.get_float("max_flip_factor"));
    else if (string == "adaptative") return std::make_shared<GJ_Adaptative_Scope>();
    
    return std::make_shared<GJ_Fixed_Scope>(1); // default
}
/* #endregion */

/* #region ======================================= GreedyJumper ======================================= */
GreedyJumper::GreedyJumper(std::shared_ptr<Selection_Criterion> criterion, std::shared_ptr<Neighborhood_Scope> scope, GreedyTrajectory::NeighborhoodOrdering positive_ordering, GreedyTrajectory::NeighborhoodOrdering negative_ordering): criterion(criterion), scope(scope), trajectory(positive_ordering, negative_ordering) {}
LocalSearchAlgo* GreedyJumper::set_seed(std::shared_ptr<std::mt19937> random_generator) {
    criterion->set_seed(random_generator);
    scope->set_seed(random_generator);
    trajectory.set_seed(random_generator);
    return LocalSearchAlgo::set_seed(random_generator);
}

bool GreedyJumper::improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget) {
    float old_score = score;
    unsigned int old_improving_neighbor_count = improving_neighbor_count;

    // create trajectory
    this->scope->create_trajectory(trajectory, instance, score, budget);

    // chose jump
    unsigned int jump_size = this->criterion->chose_jump(trajectory, instance, score, budget);
    
    improving_neighbor_count = count_better_neighbors(instance);
    output_iteration_data(budget, old_improving_neighbor_count, improving_neighbor_count, old_score, score, jump_size);
    return jump_size != 0;
}
/* #endregion */
