#include "Validator.h"

#include <map>
#include <algorithm>
#include <ranges>
#include <stdexcept>

std::map<int, std::vector<std::pair<std::chrono::sys_seconds, std::chrono::minutes>>> Validator::
get_schedule_entries() const {
    std::map<int, std::vector<std::pair<std::chrono::sys_seconds, std::chrono::minutes>>> schedule_entries {};
    for (auto manager: this->managers) {
        for (auto entry: manager.get_schedule_entries()) {
            int place_id = entry.get_place_id();
            if (!schedule_entries.contains(place_id)) {
                schedule_entries[place_id] = {};
            }
            std::pair<std::chrono::sys_seconds, std::chrono::minutes> value {};
            value.first = entry.get_timestamp();
            value.second = entry.get_duration();
            schedule_entries[place_id].push_back(value);
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

void Validator::add_manager(const ScheduleManager &Manager) {
    this->managers.push_back(Manager);
}

void Validator::add_managers(const std::vector<ScheduleManager> &Managers) {
    for (ScheduleManager manager: Managers) {
        this->add_manager(manager);
    }
}

Validator::Validator() {
    this->managers = std::vector<ScheduleManager>{};
}

Validator::Validator(const ScheduleManager &Manager) {
    this->add_manager(Manager);
}

Validator::Validator(const std::vector<ScheduleManager> &Managers) {
    this->add_managers(Managers);
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
            const auto&[fst_prev, snd_prev] = entries[i - 1];
            const auto&[fst_cur, snd_cur] = entries[i];

            const auto previous_end =
                fst_prev + snd_prev;

            if (fst_cur < previous_end) {
                return false;
            }
        }
    }

    return true;
}
