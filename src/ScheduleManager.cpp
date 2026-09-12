#include "ScheduleManager.h"

ScheduleManager::ScheduleManager() {
    this->scheduleVec = std::vector<ScheduleEntry>{};
}

bool ScheduleManager::validate() {
    return true;
}

void ScheduleManager::setId(const int &new_id) {
    const int prev_id = this->getId();
    for (auto &schedule_entry: this->scheduleVec) {
        if (schedule_entry.ownerId() == prev_id) {
            schedule_entry.own(new_id);
        }
    }
    this->id = new_id;
}

void ScheduleManager::addScheduleEntry(ScheduleEntry &new_schedule_entry) {
    if (this->idExists(new_schedule_entry.getId())) {
        throw std::runtime_error("ID #"+ std::to_string(new_schedule_entry.getId()) +" already used in '"+ this->getName() +"' manager");
    }

    if (this->nameExists(new_schedule_entry.getName())) {
        throw std::runtime_error("Name '"+ new_schedule_entry.getName() +"' already used in '"+ this->getName() +"' manager");
    }
    if (!this->idExists(new_schedule_entry.getId()) and !this->nameExists(new_schedule_entry.getName())) {
        new_schedule_entry.own(this->getId());
        this->scheduleVec.push_back(new_schedule_entry);
        return;
    }
    throw std::runtime_error("Unknown error while adding new schedule entry in '"+ this->getName() +"' manager");
}

void ScheduleManager::setSchedule(const std::vector<ScheduleEntry> &new_schedule_vec) {
    for (auto schedule_entry: new_schedule_vec) {
        this->addScheduleEntry(schedule_entry);
    }
}

void ScheduleManager::setSchedule(const std::string &name, const std::chrono::sys_seconds &timestamp,
                                  const std::chrono::seconds &duration, const int &group_id,
                                  const int &place_id) {
    auto entry = ScheduleEntry(name, timestamp, duration, group_id, place_id);
    entry.own(this->getId());
    this->addScheduleEntry(entry);
}

bool ScheduleManager::idExists(const int &target_id) const {
    for (const ScheduleEntry& schedule_entry: this->getSchedule()) {
        if (schedule_entry.getId() == target_id) return true;
    }
    return false;
}

bool ScheduleManager::nameExists(const std::string &target_name) const {
    for (ScheduleEntry schedule_entry: this->getSchedule()) {
        if (schedule_entry.getName() == target_name) return true;
    }
    return false;
}

ScheduleEntry &ScheduleManager::getEntryById(const int id) {
    for (auto &schedule_entry: this->scheduleVec) {
        if (schedule_entry.getId() == id) return schedule_entry;
    }
    throw std::runtime_error("in '" + this->getName() + "' manager #" + std::to_string(id) + " is not found");
}

ScheduleEntry & ScheduleManager::getEntryByName(const std::string &name) {
    for (auto &schedule_entry: this->scheduleVec) {
        if (schedule_entry.getName() == name) return schedule_entry;
    }
    throw std::runtime_error("in '" + this->getName() + "' manager - name '" + std::to_string(id) + "' is not found");
}
