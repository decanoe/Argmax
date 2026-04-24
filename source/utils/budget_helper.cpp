#include "budget_helper.h"

BudgetHelper::BudgetHelper(unsigned int max_budget): max_budget(max_budget), current_global_budget(0), current_in_run_budget(0) { }

unsigned int BudgetHelper::get_current_global_budget() const {
    return this->current_global_budget;
}
unsigned int BudgetHelper::get_current_in_run_budget() const {
    return this->current_in_run_budget;
}

/// @brief resets the in_run_budget to 0
void BudgetHelper::new_run() {
    this->current_in_run_budget = 0;
}
/// @brief increases the used budget by 1
BudgetHelper& BudgetHelper::operator++(int) {
    this->current_global_budget++;
    this->current_in_run_budget++;
    return *this;
}
/// @brief increases the used budget by the cost
BudgetHelper& BudgetHelper::operator+=(unsigned int cost) {
    this->current_global_budget+=cost;
    this->current_in_run_budget+=cost;
    return *this;
}

/// @return true if the max budget is reached
bool BudgetHelper::out_of_budget() const {
    return this->current_global_budget >= this->max_budget;
}
