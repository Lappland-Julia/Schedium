#include "Validator.h"

#include <map>
#include <set>
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
    if (this->managers.size() == 0) {
        return true;
    }
    else {
        for (std::map<int, std::vector<std::pair<std::chrono::sys_seconds, std::chrono::minutes>>> schedule_entries = this->get_schedule_entries(); auto &timestamp_entries: schedule_entries | std::views::values) {
            for (auto& target_timestamp: timestamp_entries) {
                bool already_matched = false;
                for (auto &matching_timestamp: timestamp_entries) {
                    // -1 - before, 0 - match, 1 - after
                    if (target_timestamp == matching_timestamp) {
                        if (already_matched == true) {
                            return false;
                        }
                        already_matched = true;
                    }
                    else {
                        std::set<std::chrono::sys_seconds> timestamp_set {};
                        int counter = 0;
                        std::chrono::seconds add_time {};
                        for (int add = 0; add < target_timestamp.second.count(); add++) {
                            add_time = std::chrono::seconds(60*add);
                            timestamp_set.insert(target_timestamp.first + add_time);
                            counter++;
                        }
                        for (int add = 0; add < matching_timestamp.second.count(); add++) {
                            add_time = std::chrono::seconds(60*add);
                            timestamp_set.insert(matching_timestamp.first + add_time);
                            counter++;
                        }
                        if (timestamp_set.size() != counter) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }
}
