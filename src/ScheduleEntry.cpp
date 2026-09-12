#include "ScheduleEntry.h"
#include "Generators.h"

void ScheduleEntry::setId(const int &new_id) {
    this->id = new_id;
}

void ScheduleEntry::setName(const std::string &new_name) {
    this->name = new_name;
}

void ScheduleEntry::setTimestamp(const std::chrono::sys_seconds &new_timestamp) {
    this->timestamp = new_timestamp;
}

void ScheduleEntry::setDuration(const std::chrono::seconds &new_duration) {
    this->duration = new_duration;
}

void ScheduleEntry::setGroupId(const int &new_group_id) {
    this->groupId = new_group_id;
}

void ScheduleEntry::setPlaceId(const int &new_place_id) {
    this->placeId = new_place_id;
}

ScheduleEntry::ScheduleEntry(const std::string &name, const std::chrono::sys_seconds &timestamp,
                             const std::chrono::seconds &duration, const int &group_id, const int &place_id) {
    this->setId(generateId());
    this->setName(name);
    this->setTimestamp(timestamp);
    this->setDuration(duration);
    this->setGroupId(group_id);
    this->setPlaceId(place_id);
}
