#ifndef ALLOC_SCHEDULE_H
#define ALLOC_SCHEDULE_H
#include <chrono>
#include <string>


class ScheduleEntry {
private:
    std::string name;
    std::chrono::minutes duration;
    std::chrono::sys_seconds start_time;
    std::string group_name = "";
    std::string place_name = "";
    bool owned = false;
    std::string manager_name = "";
public:
    const int MAX_DURATION = 6000;
    const std::chrono::minutes MAX_DURATION_TIME = std::chrono::minutes(MAX_DURATION);

    std::string get_name() const { return this->name; }
    void set_name(const std::string &new_name);

    std::chrono::sys_seconds get_timestamp() const { return this->start_time; }
    void set_timestamp(const std::chrono::sys_seconds &new_timestamp);

    std::chrono::minutes get_duration() const { return this->duration; };
    void set_duration(const std::chrono::minutes &new_duration);

    std::string get_group_name() const { return this->group_name; }
    void set_group_name(const int &new_group_name);

    std::string get_place_name() const { return this->place_name; }
    void set_place_name(const int &new_place_name);

    void own(const std::string &owner_manager_name) { this->owned = true; this->manager_name = owner_manager_name; }

    bool is_owned() const {return this->owned; }
    std::string owner_name() const {return this->manager_name;}

    ScheduleEntry (const std::string &name, const std::chrono::sys_seconds &timestamp, const std::chrono::minutes &duration, const int &group_name, const int &place_name);

};


#endif //ALLOC_SCHEDULE_H
