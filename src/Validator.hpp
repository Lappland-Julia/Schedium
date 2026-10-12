#ifndef SCHEDIUM_VALIDATOR_H
#define SCHEDIUM_VALIDATOR_H
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <cstdint>

#include "ScheduleManager.hpp"

class Validator {
private:
    std::int64_t gap_score = 5;
    std::chrono::minutes gap_duration{15};
    std::int64_t overlap_penalty = -10'000;
    std::int64_t easy_min_score = -10'000;
    std::int64_t medium_min_score = -5'000;
    std::int64_t hard_min_score = 0;
    int validating_level = 2;
    std::vector<ScheduleManager> managers = {};

    enum class VALIDATING_TYPES : int { HARD = 3, MEDIUM = 2, EASY = 1 };

    std::map<std::string, std::vector<std::pair<std::chrono::sys_seconds, std::chrono::minutes>>>
    get_schedule_entries() const;

public:
    std::int64_t get_gap_score() const { return this->gap_score; }
    void update_gap_score(const std::int64_t &new_gap_score);

    std::chrono::minutes get_gap_duration() const { return this->gap_duration; }
    void update_gap_duration(const std::chrono::minutes &new_gap_duration);

    std::int64_t get_overlap_penalty() const { return this->overlap_penalty; }
    void update_overlap_penalty(const std::int64_t &new_overlap_penalty);

    std::int64_t get_min_score() const;
    void update_min_scores(const std::int64_t &new_easy_min_score, const std::int64_t &new_medium_min_score,
                           const std::int64_t &new_hard_min_score);

    std::string get_validating_level() const;

    void set_validating_level(const std::string &new_validating_level);

    void add_manager(const ScheduleManager &new_manager);

    void add_managers(const std::vector<ScheduleManager> &new_managers);

    Validator();

    explicit Validator(const ScheduleManager &manager);

    explicit Validator(const std::vector<ScheduleManager> &schedule_managers);

    bool is_valid() const;

    std::int64_t score() const;
};

#endif // SCHEDIUM_VALIDATOR_H
