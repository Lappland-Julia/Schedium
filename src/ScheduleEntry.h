#ifndef ALLOC_SCHEDULE_H
#define ALLOC_SCHEDULE_H
#include <chrono>
#include "Generators.h"
#include <string>


class ScheduleEntry {
private:
    int id = 0;
    std::string name;
    std::chrono::seconds duration;
    std::chrono::sys_seconds timestamp;
    int groupId = 0;
    int placeId = 0;
    bool owned = false;
    int managerId = -1;

public:

    int getId() const { return this->id; }
    void set_id(const int &new_id);

    std::string get_name() const { return this->name; }
    void set_name(const std::string &new_name);

    std::chrono::sys_seconds get_timestamp() const { return this->timestamp; }
    void set_timestamp(const std::chrono::sys_seconds &new_timestamp);

    std::chrono::seconds get_duration() const { return this->duration; };
    void set_duration(const std::chrono::seconds &new_duration);

    int get_group_id() const { return this->groupId; }
    void set_group_id(const int &new_group_id);

    int get_place_id() const { return this->placeId; }
    void set_place_id(const int &new_place_id);

    void own(const int &manager_id) { this->owned = true; this->managerId = manager_id; }

    bool is_owned() const {return this->owned; }
    int owner_id() const {return this->managerId;}

    ScheduleEntry (const std::string &name, const std::chrono::sys_seconds &timestamp, const std::chrono::seconds &duration, const int &group_id, const int &place_id);

};


#endif //ALLOC_SCHEDULE_H
