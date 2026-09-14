#ifndef SCHEDIUM_VALIDATOR_H
#define SCHEDIUM_VALIDATOR_H
#include <vector>
#include <string>

#include "ScheduleManager.h"


class Validator {
private:
    int gap_score = 5;
    int validating_power = 2;
    std::vector<ScheduleManager> managers = {};
    const std::vector<std::string> VALIDATING_TYPES = {"HARD", "MEDIUM", "EASY"}; // hard - 3, medium - 2 (by default) , easy - 1
public:

    std::vector<std::string> get_validating_types() const {return this->VALIDATING_TYPES; }

    int get_gap_score() const { return this->gap_score; }
    void update_gap_score(const int &new_gap_score) {this->gap_score = new_gap_score; };

    std::string get_validating_power() const;
    void set_validating_power(const std::string &new_validating_power);

    void add_manager(const ScheduleManager &Manager);
    void add_managers(const std::vector<ScheduleManager> &Managers);

    Validator();
    explicit Validator(const ScheduleManager &Manager);
    explicit Validator(const std::vector<ScheduleManager> &Managers);
};


#endif //SCHEDIUM_VALIDATOR_H
