#ifndef ALLOC_SCHEDULE_H
#define ALLOC_SCHEDULE_H
#include <chrono>
#include <string>
#include <utility>


class ScheduleEntry {
private:
    std::string name;
    std::chrono::minutes duration;
    std::chrono::sys_seconds start_timestamp;
    std::string group_name = "";
    std::string place_name = "";
    bool owned = false;
    std::string manager_name = "";
public:
    static constexpr int MAX_DURATION = 6000;
    static constexpr auto MAX_DURATION_TIME = std::chrono::minutes(MAX_DURATION);

    const std::string& get_name() const & { return this->name; }
    std::string get_name() && { return std::move(this->name); }
    std::string get_name() const && { return this->name; }
    void set_name(const std::string &new_name);

    std::chrono::sys_seconds get_timestamp() const { return this->start_timestamp; }
    void set_timestamp(const std::chrono::sys_seconds &new_timestamp);

    std::chrono::minutes get_duration() const { return this->duration; };
    void set_duration(const std::chrono::minutes &new_duration);

    const std::string& get_group_name() const & { return this->group_name; }
    std::string get_group_name() && { return std::move(this->group_name); }
    std::string get_group_name() const && { return this->group_name; }
    void set_group_name(const std::string &new_group_name);

    const std::string& get_place_name() const & { return this->place_name; }
    std::string get_place_name() && { return std::move(this->place_name); }
    std::string get_place_name() const && { return this->place_name; }
    void set_place_name(const std::string &new_place_name);

    void own(const std::string &owner_manager_name) { this->owned = true; this->manager_name = owner_manager_name; }

    bool is_owned() const {return this->owned; }
    const std::string& owner_name() const & {return this->manager_name;}
    std::string owner_name() && { return std::move(this->manager_name); }
    std::string owner_name() const && { return this->manager_name; }

    ScheduleEntry (const std::string &name, const std::chrono::sys_seconds &timestamp, const std::chrono::minutes &duration, const std::string &group_name, const std::string &place_name);

};


#endif //ALLOC_SCHEDULE_H
