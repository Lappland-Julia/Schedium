#include "ScheduleEntry.h"

void ScheduleEntry::set_name(const std::string &new_name) {
    this->name = new_name;
}

void ScheduleEntry::set_timestamp(const std::chrono::sys_seconds &new_timestamp) {
    this->start_time = new_timestamp;
}

void ScheduleEntry::set_duration(const std::chrono::minutes &new_duration) {
    if (new_duration > this->MAX_DURATION_TIME) {
        throw std::invalid_argument("\"" + this->name + "\" entry: duration is greater than maximum allowed duration ("+std::to_string(MAX_DURATION)+" minutes)");
    }
    this->duration = new_duration;
}

void ScheduleEntry::set_group_name(const int &new_group_name) {
    this->group_name = new_group_name;
}

void ScheduleEntry::set_place_name(const int &new_place_name) {
    this->place_name = new_place_name;
}

ScheduleEntry::ScheduleEntry(const std::string &name, const std::chrono::sys_seconds &timestamp,
                             const std::chrono::minutes &duration, const int &group_name, const int &place_name) {
    this->set_name(name);
    this->set_timestamp(timestamp);
    this->set_duration(duration);
    this->set_group_name(group_name);
    this->set_place_name(place_name);
}
