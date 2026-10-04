#include "ScheduleManager.hpp"

#include <stdexcept>
#include <utility>

void ScheduleManager::add_schedule_entry(ScheduleEntry new_schedule_entry) {
    if (this->name_exists(new_schedule_entry.get_name())) {
        throw std::runtime_error("Name '"+ new_schedule_entry.get_name() +"' already used in '"+ this->get_name() +"' manager");
    }
    else {
        new_schedule_entry.own(this->get_name());
        this->schedule_vec.push_back(std::move(new_schedule_entry));
    }
}

void ScheduleManager::set_schedule(const std::vector<ScheduleEntry> &new_schedule_vec) {
    for (const auto& schedule_entry: new_schedule_vec) {
        this->add_schedule_entry(schedule_entry);
    }
}

void ScheduleManager::set_schedule(const std::string &name, const std::chrono::sys_seconds &timestamp,
                                   const std::chrono::minutes &duration, const std::string &group_name,
                                   const std::string &place_name) {
    auto entry = ScheduleEntry(name, timestamp, duration, group_name, place_name);
    this->add_schedule_entry(std::move(entry));
}

bool ScheduleManager::name_exists(const std::string &target_name) const {
    for (const auto& schedule_entry: this->schedule_vec) {
        if (schedule_entry.get_name() == target_name) return true;
    }
    return false;
}

const ScheduleEntry& ScheduleManager::get_entry_by_name(const std::string& target_name) const {
    for (const auto& schedule_entry : this->schedule_vec) {
        if (schedule_entry.get_name() == target_name) return schedule_entry;
    }
    throw std::runtime_error("in '" + this->get_name() + "' manager - name '" + target_name + "' is not found");
}

void ScheduleManager::rename_schedule_entry(const std::string& current_name, const std::string& new_name) {
    for (auto& schedule_entry : this->schedule_vec) {
        if (schedule_entry.get_name() != current_name) continue;
        if (current_name == new_name) return;
        if (this->name_exists(new_name)) {
            throw std::runtime_error("Name '" + new_name + "' already used in '" + this->get_name() + "' manager");
        }
        schedule_entry.set_name(new_name);
        return;
    }
    throw std::runtime_error("in '" + this->get_name() + "' manager - name '" + current_name + "' is not found");
}

ScheduleManager::ScheduleManager() {
    this->schedule_vec = std::vector<ScheduleEntry>{};
}
