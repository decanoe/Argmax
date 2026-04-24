#pragma once

class BudgetHelper
{
private:
    unsigned int max_budget;
    unsigned int current_global_budget;
    unsigned int current_in_run_budget;
public:
    BudgetHelper(unsigned int max_budget);
    BudgetHelper(const BudgetHelper&) = delete;

    unsigned int get_current_global_budget() const;
    unsigned int get_current_in_run_budget() const;

    /// @brief resets the in_run_budget to 0
    void new_run();
    /// @brief increases the used budget by 1
    BudgetHelper& operator++(int);
    /// @brief increases the used budget by the cost
    BudgetHelper& operator+=(unsigned int cost);

    /// @return true if the max budget is reached
    bool out_of_budget() const;
};
