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
    std::chrono::sys_seconds start_time;
    int group_id = 0;
    int place_id = 0;
    bool owned = false;
    int manager_id = -1;

public:

    int get_id() const { return this->id; }
    void set_id(const int &new_id);

    std::string get_name() const { return this->name; }
    void set_name(const std::string &new_name);

    std::chrono::sys_seconds get_timestamp() const { return this->start_time; }
    void set_timestamp(const std::chrono::sys_seconds &new_timestamp);

    std::chrono::seconds get_duration() const { return this->duration; };
    void set_duration(const std::chrono::seconds &new_duration);

    int get_group_id() const { return this->group_id; }
    void set_group_id(const int &new_group_id);

    int get_place_id() const { return this->place_id; }
    void set_place_id(const int &new_place_id);

    void own(const int &owner_manager_id) { this->owned = true; this->manager_id = owner_manager_id; }

    bool is_owned() const {return this->owned; }
    int owner_id() const {return this->manager_id;}

    ScheduleEntry (const std::string &name, const std::chrono::sys_seconds &timestamp, const std::chrono::seconds &duration, const int &group_id, const int &place_id);

};


#endif //ALLOC_SCHEDULE_H
