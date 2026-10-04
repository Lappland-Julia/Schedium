#ifndef ALLOC_SCHEDULEMANAGER_H
#define ALLOC_SCHEDULEMANAGER_H
#include <vector>
#include "ScheduleEntry.hpp"


class ScheduleManager {
private:
    std::string name = "Schedule manager";
    std::vector<ScheduleEntry> schedule_vec;

public:
    std::string get_name() const { return this->name; }
    void set_name(const std::string &new_name) { this->name = new_name; }

    std::vector<ScheduleEntry> get_schedule() const { return this->schedule_vec; }

    void add_schedule_entry(ScheduleEntry &new_schedule_entry);
    void set_schedule(const std::vector<ScheduleEntry> &new_schedule_vec);

    void set_schedule(const std::string &name, const std::chrono::sys_seconds &timestamp, const std::chrono::minutes &duration, const std::string &group_name, const std::string &place_name);
    std::vector<ScheduleEntry> get_schedule_entries() const { return this->schedule_vec; }

    bool name_exists(const std::string &target_name) const;

    const ScheduleEntry & get_entry_by_name(const std::string& target_name) const;
    void rename_schedule_entry(const std::string& current_name, const std::string& new_name);

    ScheduleManager ();
    explicit ScheduleManager (const std::vector<ScheduleEntry> &schedule_entries) { this->set_schedule(schedule_entries); }
};


#endif //ALLOC_SCHEDULEMANAGER_H
