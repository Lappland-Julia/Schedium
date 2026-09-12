#include "ScheduleManager.h"

ScheduleManager::ScheduleManager() {
    this->schedule_vec = std::vector<ScheduleEntry>{};
}

void ScheduleManager::set_id(const int &new_id) {
    const int prev_id = this->get_id();
    for (auto &schedule_entry: this->schedule_vec) {
        if (schedule_entry.ownerId() == prev_id) {
            schedule_entry.own(new_id);
        }
    }
    this->id = new_id;
}

void ScheduleManager::add_schedule_entry(ScheduleEntry &new_schedule_entry) {
    if (this->id_exists(new_schedule_entry.getId())) {
        throw std::runtime_error("ID #"+ std::to_string(new_schedule_entry.getId()) +" already used in '"+ this->get_name() +"' manager");
    }

    if (this->name_exists(new_schedule_entry.getName())) {
        throw std::runtime_error("Name '"+ new_schedule_entry.getName() +"' already used in '"+ this->get_name() +"' manager");
    }
    if (!this->id_exists(new_schedule_entry.getId()) and !this->name_exists(new_schedule_entry.getName())) {
        new_schedule_entry.own(this->get_id());
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
                                  const std::chrono::seconds &duration, const int &group_id,
                                  const int &place_id) {
    auto entry = ScheduleEntry(name, timestamp, duration, group_id, place_id);
    entry.own(this->get_id());
    this->add_schedule_entry(entry);
}

bool ScheduleManager::id_exists(const int &target_id) const {
    for (const ScheduleEntry& schedule_entry: this->get_schedule()) {
        if (schedule_entry.getId() == target_id) return true;
    }
    return false;
}

bool ScheduleManager::name_exists(const std::string &target_name) const {
    for (ScheduleEntry schedule_entry: this->get_schedule()) {
        if (schedule_entry.getName() == target_name) return true;
    }
    return false;
}

ScheduleEntry &ScheduleManager::get_entry_by_id(const int id) {
    for (auto &schedule_entry: this->schedule_vec) {
        if (schedule_entry.getId() == id) return schedule_entry;
    }
    throw std::runtime_error("in '" + this->get_name() + "' manager #" + std::to_string(id) + " is not found");
}

ScheduleEntry & ScheduleManager::get_entry_by_name(const std::string &name) {
    for (auto &schedule_entry: this->schedule_vec) {
        if (schedule_entry.getName() == name) return schedule_entry;
    }
    throw std::runtime_error("in '" + this->get_name() + "' manager - name '" + std::to_string(id) + "' is not found");
}
