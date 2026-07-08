#include "greedy_jumper.h"

using namespace LocalSearch;

/* #region ======================================= GreedyJumper::Selection_Criterion ======================================= */
class GJ_Best_Criterion: public GreedyJumper::Selection_Criterion {
public:
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score, unsigned int iteration) const override {
        return (tested_score > init_score) && (first_keep || (tested_score > iteration_best_score));
    }
    bool stop_at_first_improve(unsigned int iteration) const override {
        return false;
    }
protected:
};
class GJ_Least_Criterion: public GreedyJumper::Selection_Criterion {
public:
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score, unsigned int iteration) const override {
        return (tested_score > init_score) && (first_keep || (tested_score < iteration_best_score));
    }
    bool stop_at_first_improve(unsigned int iteration) const override {
        return false;
    }
protected:
};
class GJ_First_Criterion: public GreedyJumper::Selection_Criterion {
public:
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score, unsigned int iteration) const override {
        return (tested_score > init_score);
    }
    bool stop_at_first_improve(unsigned int iteration) const override {
        return true;
    }
protected:
};
class GJ_Random_Criterion: public GJ_First_Criterion {
public:
    unsigned int chose_jump(const GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float& instance_score, BudgetHelper& budget, unsigned int iteration) const override {
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
class GJ_MiddleScore_Criterion: public GreedyJumper::Selection_Criterion {
public:
    unsigned int chose_jump(const GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float& instance_score, BudgetHelper& budget, unsigned int iteration) const override {
        std::vector<float> results = std::vector<float>(trajectory.size() + 1, 0); // index 0 not used (no jump of size 0)
        
        // apply all mutations in trajectory and gather max and min improvements
        unsigned int count = 0;
        float max = instance_score - 1;
        float min = instance_score - 1;
        for (const BitFlip& bitflip : trajectory) {
            count++;
            instance->mutate_arg(bitflip.index);
            budget++;
            float s = instance->score();
            results[count] = s;
            if (s > max) max = s;
            if (min < instance_score || (s < min && s > instance_score)) min = s;
        }
        if (max <= instance_score) { // revert instance and return 0 (no improvement found)
            for (auto bitflip = trajectory.rbegin(); bitflip != trajectory.rend(); ++bitflip) instance->mutate_arg((*bitflip).index);
            return 0;
        }

        // find jump closest to target
        float target_score = (max + min) / 2;
        unsigned int best_count = 1;
        for (unsigned int i = 2; i <= trajectory.size(); ++i) {
            if (std::abs(target_score - results[i]) < std::abs(target_score - results[best_count])) best_count = i;
        }
        
        // apply best jump
        for (auto bitflip = trajectory.rbegin(); bitflip != trajectory.rend(); ++bitflip) {
            if (count == best_count) break;
            instance->mutate_arg((*bitflip).index);
            count--;
        }

        instance_score = results[best_count];
        return best_count;
    }

    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score, unsigned int iteration) const override {
        return tested_score > init_score; // not used
    }
    bool stop_at_first_improve(unsigned int iteration) const override {
        return false; // not used
    }
protected:
};
class GJ_MiddleJump_Criterion: public GreedyJumper::Selection_Criterion {
public:
    unsigned int chose_jump(const GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float& instance_score, BudgetHelper& budget, unsigned int iteration) const override {
        std::vector<float> results = std::vector<float>(trajectory.size() + 1, 0); // index 0 not used (no jump of size 0)
        
        // apply all mutations in trajectory and gather max and min improvements
        unsigned int count = 0;
        unsigned int max_count = -1U;
        unsigned int min_count = -1U;
        float max = instance_score;
        float min = instance_score;
        for (const BitFlip& bitflip : trajectory) {
            count++;
            instance->mutate_arg(bitflip.index);
            budget++;
            float s = instance->score();
            results[count] = s;
            if (s <= instance_score) continue;

            if (max_count == -1U || s > max) {
                max_count = count;
                max = s;
            }
            if (min_count == -1U || s < min) {
                min_count = count;
                min = s;
            }
        }
        if (max_count == -1U) { // revert instance and return 0 (no improvement found)
            for (auto bitflip = trajectory.rbegin(); bitflip != trajectory.rend(); ++bitflip) instance->mutate_arg((*bitflip).index);
            return 0;
        }

        // apply average of best jump and worst jump
        unsigned int target_count = std::ceil((max_count + min_count) / 2.f);
        for (auto bitflip = trajectory.rbegin(); bitflip != trajectory.rend(); ++bitflip) {
            if (count == target_count) break;
            instance->mutate_arg((*bitflip).index);
            count--;
        }

        instance_score = results[target_count];
        return target_count;
    }

    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score, unsigned int iteration) const override {
        return tested_score > init_score; // not used
    }
    bool stop_at_first_improve(unsigned int iteration) const override {
        return false; // not used
    }
protected:
};
class GJ_MedianScore_Criterion: public GreedyJumper::Selection_Criterion {
public:
    unsigned int chose_jump(const GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float& instance_score, BudgetHelper& budget, unsigned int iteration) const override {
        std::vector<std::pair<float, unsigned int>> improving_jumps = std::vector<std::pair<float, unsigned int>>();
        
        // apply all mutations in trajectory and gather max and min improvements
        unsigned int count = 0;
        for (const BitFlip& bitflip : trajectory) {
            count++;
            instance->mutate_arg(bitflip.index);
            budget++;
            float s = instance->score();
            if (s > instance_score) improving_jumps.push_back(std::pair<float, unsigned int>(s, count));
        }
        if (improving_jumps.empty()) { // revert instance and return 0 (no improvement found)
            for (auto bitflip = trajectory.rbegin(); bitflip != trajectory.rend(); ++bitflip) instance->mutate_arg((*bitflip).index);
            return 0;
        }

        // find jump size for median (improving) score
        auto median = improving_jumps.begin() + improving_jumps.size() / 2;
        std::nth_element(improving_jumps.begin(), median, improving_jumps.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

        // apply target jump size
        for (auto bitflip = trajectory.rbegin(); bitflip != trajectory.rend(); ++bitflip) {
            if (count == median->second) break;
            instance->mutate_arg((*bitflip).index);
            count--;
        }

        instance_score = median->first;
        return median->second;
    }

    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score, unsigned int iteration) const override {
        return tested_score > init_score; // not used
    }
    bool stop_at_first_improve(unsigned int iteration) const override {
        return false; // not used
    }
protected:
};
class GJ_Guided_Criterion: public GreedyJumper::Selection_Criterion {
public:
    GJ_Guided_Criterion(std::shared_ptr<GreedyJumper::Selection_Criterion> base_criterion, std::shared_ptr<GreedyJumper::Selection_Criterion> guide_criterion, unsigned int guide_turn_count):
        GreedyJumper::Selection_Criterion(), base_criterion(base_criterion), guide_criterion(guide_criterion), guide_turn_count(guide_turn_count) {}

    unsigned int chose_jump(const GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float& instance_score, BudgetHelper& budget, unsigned int iteration) const override {
        if (iteration < guide_turn_count) return guide_criterion->chose_jump(trajectory, instance, instance_score, budget, iteration);
        return base_criterion->chose_jump(trajectory, instance, instance_score, budget, iteration);
    }
    bool do_keep(float tested_score, float init_score, bool first_keep, float iteration_best_score, unsigned int iteration) const override {
        if (iteration < guide_turn_count) return guide_criterion->do_keep(tested_score, init_score, first_keep, iteration_best_score, iteration);
        return base_criterion->do_keep(tested_score, init_score, first_keep, iteration_best_score, iteration);
    }
    bool stop_at_first_improve(unsigned int iteration) const override {
        if (iteration < guide_turn_count) return guide_criterion->stop_at_first_improve(iteration);
        return base_criterion->stop_at_first_improve(iteration);
    }

    LocalSearchAlgoComponent* set_seed(std::shared_ptr<std::mt19937> random_generator) override {
        base_criterion->set_seed(random_generator);
        guide_criterion->set_seed(random_generator);
        return GreedyJumper::Selection_Criterion::set_seed(random_generator);
    }
protected:
    std::shared_ptr<GreedyJumper::Selection_Criterion> base_criterion, guide_criterion;
    unsigned int guide_turn_count;
};

unsigned int GreedyJumper::Selection_Criterion::chose_jump(const GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float& instance_score, BudgetHelper& budget, unsigned int iteration) const {
    // apply all mutations in trajectory
    for (const BitFlip& bitflip : trajectory) instance->mutate_arg(bitflip.index);
    
    // compare jumps of the trajectory
    unsigned int count = trajectory.size();
    unsigned int best_count = 0;
    float best_score = instance_score;
    for (auto bitflip = trajectory.rbegin(); bitflip != trajectory.rend(); ++bitflip) {
        budget++;
        if (this->do_keep(instance->score(), instance_score, best_count == 0, best_score, iteration)) {
            best_score = instance->score();
            best_count = count;
            if (this->stop_at_first_improve(iteration)) break;
        }
        count--;
        instance->mutate_arg((*bitflip).index);
    }

    // apply best jump found
    instance_score = best_score;
    if (!this->stop_at_first_improve(iteration)) {
        count = best_count;
        for (const BitFlip& bitflip : trajectory) {
            if (count == 0) break;
            count--;
            instance->mutate_arg(bitflip.index);
        }
    }

    return best_count;
}
std::shared_ptr<GreedyJumper::Selection_Criterion> GreedyJumper::Selection_Criterion::from_values(const std::string& key) {
    if (key == "first") return std::make_shared<GJ_First_Criterion>();
    else if (key == "best") return std::make_shared<GJ_Best_Criterion>();
    else if (key == "least") return std::make_shared<GJ_Least_Criterion>();
    else if (key == "random") return std::make_shared<GJ_Random_Criterion>();
    else if (key == "middle") return std::make_shared<GJ_MiddleScore_Criterion>();
    else if (key == "middle2") return std::make_shared<GJ_MiddleJump_Criterion>();
    else if (key == "median") return std::make_shared<GJ_MedianScore_Criterion>();
    
    std::cerr << "\033[1;31mThe field <selection_criterion> or <guide_criterion> \"" << key << "\" of the preset file doesn't correspond to any implemented GreedyJumper::Selection_Criterion.\n\033[0m";
    exit(1);
}
std::shared_ptr<GreedyJumper::Selection_Criterion> GreedyJumper::Selection_Criterion::from_file_data(const Parameters& parameters) {
    if (parameters.contains_string("guide_criterion")) {
        return std::make_shared<GJ_Guided_Criterion>(
            from_values(parameters.get_string("selection_criterion")),
            from_values(parameters.get_string("guide_criterion")),
            parameters.get_int("guide_turn_count")
        );
    }
    return from_values(parameters.get_string("selection_criterion"));
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
    GJ_Adaptative_Scope(unsigned int min_size, unsigned int factor, unsigned int constant): GreedyJumper::Neighborhood_Scope(), min_size(min_size), factor(factor), constant(constant) {}

    void create_trajectory(GreedyTrajectory& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score, BudgetHelper& budget, std::function<bool(unsigned int)> is_valid) const override {
        add_bit_flips(trajectory, instance, score, budget, is_valid);
        trajectory.finalize(std::max(min_size, factor * trajectory.positive_size() + constant));
    }
protected:
    unsigned int min_size;
    unsigned int factor;
    unsigned int constant;
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
            trajectory.insert_positive_flip(i, instance->score());
        else
            trajectory.insert_negative_flip(i, instance->score());
        
        instance->revert_last_mutation();
    }
}

std::shared_ptr<GreedyJumper::Neighborhood_Scope> GreedyJumper::Neighborhood_Scope::from_file_data(const Parameters& parameters) {
    std::string string = parameters.get_string("neighborhood_scope");
    if (string == "improve") return std::make_shared<GJ_Improve_Scope>();
    else if (string == "all") return std::make_shared<GJ_Fixed_Scope>(1);
    else if (string == "half") return std::make_shared<GJ_Fixed_Scope>(.5);
    else if (string == "fixed") return std::make_shared<GJ_Fixed_Scope>(parameters.get_float("max_flip_factor"));
    else if (string == "adaptative") return std::make_shared<GJ_Adaptative_Scope>(
        parameters.get_int("min_size", 5),
        parameters.get_int("factor", 1),
        parameters.get_int("constant", 0)
    );
    
    std::cerr << "\033[1;31mThe field <neighborhood_scope> \"" << string << "\" of the preset file doesn't correspond to any implemented GreedyJumper::Neighborhood_Scope.\n\033[0m";
    exit(1);
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

bool GreedyJumper::improve(std::unique_ptr<ReversibleInstance>& instance, float& score, unsigned int& improving_neighbor_count, BudgetHelper& budget, unsigned int iteration) {
    float old_score = score;
    unsigned int old_improving_neighbor_count = improving_neighbor_count;

    // create trajectory
    this->scope->create_trajectory(trajectory, instance, score, budget);

    // chose jump
    unsigned int jump_size = this->criterion->chose_jump(trajectory, instance, score, budget, iteration);
    
    improving_neighbor_count = count_better_neighbors(instance);
    output_iteration_data(budget, old_improving_neighbor_count, improving_neighbor_count, old_score, score, jump_size);
    return jump_size != 0;
}
/* #endregion */
