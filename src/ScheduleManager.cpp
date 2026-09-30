#include "ScheduleManager.h"

void ScheduleManager::add_schedule_entry(ScheduleEntry &new_schedule_entry) {
    bool name_exists = this->name_exists(new_schedule_entry.get_name());
    if (name_exists) {
        throw std::runtime_error("Name '"+ new_schedule_entry.get_name() +"' already used in '"+ this->get_name() +"' manager");
    }
    else {
        new_schedule_entry.own(this->get_name());
        this->schedule_vec.push_back(new_schedule_entry);
        return;
    }
    throw std::runtime_error("Unknown error while adding new schedule entry in '"+ this->get_name() +"' manager");
}

void ScheduleManager::set_schedule(const std::vector<ScheduleEntry> &new_schedule_vec) {
    for (auto schedule_entry: new_schedule_vec) {
        this->add_schedule_entry(schedule_entry);
    }
}

void ScheduleManager::set_schedule(const std::string &name, const std::chrono::sys_seconds &timestamp,
                                   const std::chrono::minutes &duration, const int &group_name,
                                   const int &place_name) {
    auto entry = ScheduleEntry(name, timestamp, duration, group_name, place_name);
    entry.own(this->get_name());
    this->add_schedule_entry(entry);
}

bool ScheduleManager::name_exists(const std::string &target_name) const {
    for (ScheduleEntry schedule_entry: this->get_schedule()) {
        if (schedule_entry.get_name() == target_name) return true;
    }
    return false;
}

ScheduleEntry & ScheduleManager::get_entry_by_name(const std::string &name) {
    for (auto &schedule_entry: this->schedule_vec) {
        if (schedule_entry.get_name() == name) return schedule_entry;
    }
    throw std::runtime_error("in '" + this->get_name() + "' manager - name '" + name + "' is not found");
}

ScheduleManager::ScheduleManager() {
    this->schedule_vec = std::vector<ScheduleEntry>{};
}
