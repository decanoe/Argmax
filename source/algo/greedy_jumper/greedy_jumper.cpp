#include "greedy_jumper.h"

auto cmp = [](std::pair<unsigned int, float> a, std::pair<unsigned int, float> b) {
    if (a.second != b.second) return a.second > b.second;
    else return a.first < b.first;
};

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

std::shared_ptr<GreedyJumper::Selection_Criterion> GreedyJumper::Selection_Criterion::from_string(const std::string& string) {
    if (string == "first") return std::make_shared<GJ_First_Criterion>();
    else if (string == "best") return std::make_shared<GJ_Best_Criterion>();
    else if (string == "least") return std::make_shared<GJ_Least_Criterion>();
    
    return std::make_shared<GJ_First_Criterion>(); // default
}
/* #endregion */
/* #region ======================================= GreedyJumper::Selection_Criterion ======================================= */
class GJ_Full_Scope: public GreedyJumper::Neighborhood_Scope {
public:
    void reset() {}
    unsigned int create_trajectory(GreedyJumper::TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score) override {
        // create trajectory
        trajectory.clear();
        unsigned int used_budget = 0;
        for (size_t i = 0; i < instance->nb_args(); i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            trajectory.insert({i, instance->score()});
            instance->revert_last_mutation();
        }
        return used_budget;
    }
protected:
};
class GJ_Improve_Scope: public GreedyJumper::Neighborhood_Scope {
public:
    void reset() {}
    unsigned int create_trajectory(GreedyJumper::TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score) override {
        // create trajectory
        trajectory.clear();
        unsigned int used_budget = 0;
        for (size_t i = 0; i < instance->nb_args(); i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            if (instance->score() > score) trajectory.insert({i, instance->score()});
            instance->revert_last_mutation();
        }
        return used_budget;
    }
protected:
};
class GJ_Half_Scope: public GreedyJumper::Neighborhood_Scope {
public:
    void reset() {}
    unsigned int create_trajectory(GreedyJumper::TrajectorySet& trajectory, std::unique_ptr<ReversibleInstance>& instance, float score) override {
        // create trajectory
        trajectory.clear();
        unsigned int used_budget = 0;
        for (size_t i = 0; i < instance->nb_args(); i++)
        {
            instance->mutate_arg(i);
            used_budget++;
            trajectory.insert({i, instance->score()});
            instance->revert_last_mutation();
        }
        while (trajectory.size() > instance->nb_args() / 2) trajectory.erase(std::next(trajectory.rbegin()).base());
        return used_budget;
    }
protected:
};

std::shared_ptr<GreedyJumper::Neighborhood_Scope> GreedyJumper::Neighborhood_Scope::from_string(const std::string& string) {
    if (string == "improve") return std::make_shared<GJ_Improve_Scope>();
    else if (string == "all") return std::make_shared<GJ_Full_Scope>();
    else if (string == "half") return std::make_shared<GJ_Half_Scope>();
    
    return std::make_shared<GJ_Full_Scope>(); // default
}
/* #endregion */

/* #region ======================================= GreedyJumper ======================================= */
GreedyJumper::GreedyJumper(std::shared_ptr<Selection_Criterion> criterion, std::shared_ptr<Neighborhood_Scope> scope): criterion(criterion), scope(scope) {}

unsigned int GreedyJumper::improve(std::unique_ptr<ReversibleInstance>& instance, unsigned int budget, unsigned int initial_budget) const {
    float score = instance->score();
    unsigned int used_budget = 1 + initial_budget;
    unsigned int better_neighbors = count_better_neighbors(instance);
    unsigned int better_neighbors_after = better_neighbors;
    output_iteration_ends_data(used_budget, (used_budget  - initial_budget), better_neighbors, score);

    GreedyJumper::TrajectorySet trajectory(cmp);
    this->scope->reset();
    while (used_budget < budget)
    {
        // create trajectory
        used_budget += this->scope->create_trajectory(trajectory, instance, score);

        // apply all mutations in trajectory
        for (std::pair<unsigned int, float> pair : trajectory) instance->mutate_arg(pair.first);

        // compare jumps of the trajectory
        unsigned int count = trajectory.size();
        unsigned int iteration_best_count = 0;
        float iteration_best_score = score;
        for (auto pair = trajectory.rbegin(); pair != trajectory.rend(); pair++) {
            used_budget++;
            if (this->criterion->do_keep(instance->score(), score, iteration_best_count == 0, iteration_best_score)) {
                iteration_best_score = instance->score();
                iteration_best_count = count;
                if (this->criterion->stop_at_first_improve()) break;
            }
            count--;
            instance->mutate_arg(pair->first);
        }

        // apply best jump found
        float old_score = score;
        score = iteration_best_score;
        if (!this->criterion->stop_at_first_improve()) {
            count = iteration_best_count;
            for (std::pair<unsigned int, float> pair : trajectory) {
                if (count == 0) break;
                count--;
                instance->mutate_arg(pair.first);
            }
        }
        
        better_neighbors_after = count_better_neighbors(instance);
        output_iteration_data(used_budget, (used_budget  - initial_budget), better_neighbors, better_neighbors_after, old_score, score, iteration_best_count);
        better_neighbors = better_neighbors_after;
        if (iteration_best_count == 0) return used_budget;
    }
    return used_budget;
}
/* #endregion */
