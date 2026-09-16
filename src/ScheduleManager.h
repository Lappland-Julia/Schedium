#ifndef ALLOC_SCHEDULEMANAGER_H
#define ALLOC_SCHEDULEMANAGER_H
#include <vector>
#include "ScheduleEntry.h"
#include "Generators.h"


class ScheduleManager {
private:
    int id = generate_id();
    std::string name = "Schedule manager";
    std::vector<ScheduleEntry> schedule_vec;

public:
    int get_id() const { return this->id; }
    void set_id(const int &new_id);

    std::string get_name() { return this->name; }
    void set_name(const std::string &new_name) { this->name = new_name; }

    std::vector<ScheduleEntry> get_schedule() const { return this->schedule_vec; }

    void add_schedule_entry(ScheduleEntry &new_schedule_entry);
    void set_schedule(const std::vector<ScheduleEntry> &new_schedule_vec);

    void set_schedule(const std::string &name, const std::chrono::sys_seconds &timestamp, const std::chrono::seconds &duration, const int &group_id, const int &place_id);
    std::vector<ScheduleEntry> get_schedule_entries() const { return this->schedule_vec; }

    bool id_exists(const int &target_id) const;
    bool name_exists(const std::string &target_name) const;

    ScheduleEntry &get_entry_by_id(const int id);
    ScheduleEntry &get_entry_by_name(const std::string &name);

    ScheduleManager ();
    explicit ScheduleManager (const std::vector<ScheduleEntry> &schedule_entries) { this->set_schedule(schedule_entries); }
};


#endif //ALLOC_SCHEDULEMANAGER_H
