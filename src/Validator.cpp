#include "Validator.hpp"

#include <map>
#include <algorithm>
#include <ranges>
#include <stdexcept>

std::map<std::string, std::vector<std::pair<std::chrono::sys_seconds, std::chrono::minutes>>> Validator::
get_schedule_entries() const {
    std::map<std::string, std::vector<std::pair<std::chrono::sys_seconds, std::chrono::minutes>>> schedule_entries {};
    for (const auto& manager: this->managers) {
        for (const auto& entry: manager.get_schedule_entries()) {
            const auto& place_name = entry.get_place_name();
            schedule_entries[place_name].emplace_back(entry.get_timestamp(), entry.get_duration());
        }
    }
    return schedule_entries;
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
    }
    else if (new_validating_level == "MEDIUM") {
        this->validating_level = 2;
    }
    else if (new_validating_level == "HARD") {
        this->validating_level = 3;
    }
    else {
        throw std::invalid_argument("Invalid validating level");
    }
}

void Validator::add_manager(const ScheduleManager &new_manager) {
    this->managers.push_back(new_manager);
}

void Validator::add_managers(const std::vector<ScheduleManager> &new_managers) {
    for (const auto& manager: new_managers) {
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
    if (managers.empty()) {
        return true;
    }

    for (auto schedule_entries = get_schedule_entries(); auto& entries : schedule_entries | std::views::values) {
        std::ranges::sort(
            entries,
            {},
            [](const auto& entry) {
                return entry.first;
            }
        );

        for (std::size_t i = 1; i < entries.size(); ++i) {
            const auto& [previous_start, previous_duration] = entries[i - 1];
            const auto current_start = entries[i].first;

            const auto previous_end = previous_start + previous_duration;

            if (current_start < previous_end) {
                return false;
            }
        }
    }

    return true;
}
