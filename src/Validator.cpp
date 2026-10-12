#include "Validator.hpp"

#include <map>
#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <limits>

std::map<std::string, std::vector<std::pair<std::chrono::sys_seconds, std::chrono::minutes>>>
Validator::get_schedule_entries() const {
    std::map<std::string, std::vector<std::pair<std::chrono::sys_seconds, std::chrono::minutes>>> schedule_entries{};
    for (const auto &manager : this->managers) {
        for (const auto &entry : manager.get_schedule_entries()) {
            const auto &place_name = entry.get_place_name();
            schedule_entries[place_name].emplace_back(entry.get_timestamp(), entry.get_duration());
        }
    }
    return schedule_entries;
}

void Validator::update_gap_score(const std::int64_t &new_gap_score) {
    if (new_gap_score < 0) {
        throw std::invalid_argument("Gap score must not be negative");
    }
    this->gap_score = new_gap_score;
}

void Validator::update_gap_duration(const std::chrono::minutes &new_gap_duration) {
    if (new_gap_duration < std::chrono::minutes{0} ||
        new_gap_duration > std::chrono::duration_cast<std::chrono::minutes>(std::chrono::seconds::max())) {
        throw std::invalid_argument("Gap duration is outside the supported range");
    }
    this->gap_duration = new_gap_duration;
}

void Validator::update_overlap_penalty(const std::int64_t &new_overlap_penalty) {
    if (new_overlap_penalty > 0) {
        throw std::invalid_argument("Overlap penalty must not be positive");
    }
    this->overlap_penalty = new_overlap_penalty;
}

std::int64_t Validator::get_min_score() const {
    switch (this->validating_level) {
        case 1:
            return this->easy_min_score;
        case 2:
            return this->medium_min_score;
        case 3:
            return this->hard_min_score;
        default:
            throw std::invalid_argument("Invalid validating level");
    }
}

void Validator::update_min_scores(const std::int64_t &new_easy_min_score, const std::int64_t &new_medium_min_score,
                                  const std::int64_t &new_hard_min_score) {
    if (new_easy_min_score > new_medium_min_score || new_medium_min_score > new_hard_min_score) {
        throw std::invalid_argument("Minimum scores must follow EASY <= MEDIUM <= HARD");
    }
    this->easy_min_score = new_easy_min_score;
    this->medium_min_score = new_medium_min_score;
    this->hard_min_score = new_hard_min_score;
}

std::string Validator::get_validating_level() const {
    switch (this->validating_level) {
        case 1:
            return "EASY";
            break;
        case 2:
            return "MEDIUM";
            break;
        case 3:
            return "HARD";
            break;
        default:
            return "UNKNOWN";
    }
}

void Validator::set_validating_level(const std::string &new_validating_level) {
    if (new_validating_level == "EASY") {
        this->validating_level = 1;
    } else if (new_validating_level == "MEDIUM") {
        this->validating_level = 2;
    } else if (new_validating_level == "HARD") {
        this->validating_level = 3;
    } else {
        throw std::invalid_argument("Invalid validating level");
    }
}

void Validator::add_manager(const ScheduleManager &new_manager) {
    this->managers.push_back(new_manager);
}

void Validator::add_managers(const std::vector<ScheduleManager> &new_managers) {
    for (const auto &manager : new_managers) {
        this->add_manager(manager);
    }
}

Validator::Validator() {
    this->managers = std::vector<ScheduleManager>{};
}

Validator::Validator(const ScheduleManager &manager) {
    this->add_manager(manager);
}

Validator::Validator(const std::vector<ScheduleManager> &schedule_managers) {
    this->add_managers(schedule_managers);
}

bool Validator::is_valid() const {
    return this->score() >= this->get_min_score();
}

std::int64_t Validator::score() const {
    std::int64_t schedule_score = 0;
    const auto gap_seconds = std::chrono::duration_cast<std::chrono::seconds>(this->gap_duration);

    if (managers.empty()) {
        return schedule_score;
    }

    for (auto schedule_entries = get_schedule_entries(); auto &entries : schedule_entries | std::views::values) {
        std::ranges::sort(entries, {}, [](const auto &entry) { return entry.first; });

        auto latest_end = std::chrono::sys_seconds{};
        for (std::size_t i = 0; i < entries.size(); ++i) {
            const auto &[current_start, current_duration] = entries[i];
            if (current_start > std::chrono::sys_seconds::max() - current_duration) {
                throw std::overflow_error("Entry end is outside the timestamp range");
            }
            const auto current_end = current_start + current_duration;

            if (i > 0 && latest_end <= std::chrono::sys_seconds::max() - gap_seconds &&
                current_start > latest_end + gap_seconds) {
                if (schedule_score > std::numeric_limits<std::int64_t>::max() - this->gap_score) {
                    throw std::overflow_error("Schedule score is outside the supported range");
                }
                schedule_score += this->gap_score;
            }

            for (std::size_t j = i + 1; j < entries.size(); ++j) {
                if (entries[j].first >= current_end) {
                    break;
                }
                if (schedule_score < std::numeric_limits<std::int64_t>::min() - this->overlap_penalty) {
                    throw std::overflow_error("Schedule score is outside the supported range");
                }
                schedule_score += this->overlap_penalty;
            }

            if (i == 0 || current_end > latest_end) {
                latest_end = current_end;
            }
        }
    }

    return schedule_score;
}
