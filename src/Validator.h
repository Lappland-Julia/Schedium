#ifndef SCHEDIUM_VALIDATOR_H
#define SCHEDIUM_VALIDATOR_H
#include <vector>
#include <string>

#include "ScheduleManager.h"


class Validator {
private:
    int gap_score = 5;
    int validating_level = 2;
    std::vector<ScheduleManager> managers = {};
    enum class VALIDATING_TYPES: int { HARD = 3, MEDIUM = 2, EASY = 1 };
public:

    int get_gap_score() const { return this->gap_score; }
    void update_gap_score(const int &new_gap_score) {this->gap_score = new_gap_score; };

    std::string get_validating_level() const;
    void set_validating_level(const std::string &new_validating_level);

    void add_manager(const ScheduleManager &Manager);
    void add_managers(const std::vector<ScheduleManager> &Managers);

    Validator();
    explicit Validator(const ScheduleManager &Manager);
    explicit Validator(const std::vector<ScheduleManager> &Managers);

    bool is_valid() const;
};


#endif //SCHEDIUM_VALIDATOR_H
