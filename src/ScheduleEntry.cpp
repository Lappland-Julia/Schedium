#include "ScheduleEntry.h"
#include "Generators.h"

void ScheduleEntry::set_id(const int &new_id) {
    this->id = new_id;
}

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

void ScheduleEntry::set_group_id(const int &new_group_id) {
    this->group_id = new_group_id;
}

void ScheduleEntry::set_place_id(const int &new_place_id) {
    this->place_id = new_place_id;
}

ScheduleEntry::ScheduleEntry(const std::string &name, const std::chrono::sys_seconds &timestamp,
                             const std::chrono::minutes &duration, const int &group_id, const int &place_id) {
    this->set_id(generate_id());
    this->set_name(name);
    this->set_timestamp(timestamp);
    this->set_duration(duration);
    this->set_group_id(group_id);
    this->set_place_id(place_id);
}
