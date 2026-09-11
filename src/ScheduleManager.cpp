#include "ScheduleManager.h"

void ScheduleManager::addScheduleEntry(const ScheduleEntry &new_schedule_entry) {
    if (this->idExists(new_schedule_entry.getId())) {
        throw std::runtime_error("ID #"+ std::to_string(new_schedule_entry.getId()) +" already used in '"+ this->getName() +"' manager");
    }

    if (this->nameExists(new_schedule_entry.getName())) {
        throw std::runtime_error("Name '"+ new_schedule_entry.getName() +"' already used in '"+ this->getName() +"' manager");
    }
    if (!this->idExists(new_schedule_entry.getId()) and !this->nameExists(new_schedule_entry.getName())) {
        this->scheduleVec.push_back(new_schedule_entry);
        return;
    }
    throw std::runtime_error("Unknown error while adding new schedule entry in '"+ this->getName() +"' manager");
}

bool ScheduleManager::idExists(const int &target_id) const {
    for (ScheduleEntry schedule_entry: this->scheduleVec) {
        if (schedule_entry.getId() == target_id) return true;
    }
    return false;
}

bool ScheduleManager::nameExists(const std::string &target_name) const {
    for (ScheduleEntry schedule_entry: this->scheduleVec) {
        if (schedule_entry.getName() == target_name) return true;
    }
    return false;
}
