#ifndef ALLOC_SCHEDULEMANAGER_H
#define ALLOC_SCHEDULEMANAGER_H
#include <vector>
#include "ScheduleEntry.h"
#include "Generators.h"


class ScheduleManager {
private:
    int id = generateId();
    std::string name = "Schedule manager";
    std::vector<ScheduleEntry> scheduleVec;

public:
    int getId() const { return this->id; }
    void setId(const int &new_id);

    std::string getName() { return this->name; }
    void setName(const std::string &new_name) { this->name = new_name; }

    std::vector<ScheduleEntry> getSchedule() const { return this->scheduleVec; }

    void addScheduleEntry(ScheduleEntry &new_schedule_entry);
    void setSchedule(const std::vector<ScheduleEntry> &new_schedule_vec);

    void setSchedule(const std::string &name, const std::chrono::sys_seconds &timestamp, const std::chrono::seconds &duration, const int &group_id, const int &place_id);

    bool idExists(const int &target_id) const;
    bool nameExists(const std::string &target_name) const;

    ScheduleEntry &getEntryById(const int id);
    ScheduleEntry &getEntryByName(const std::string &name);

    ScheduleManager ();
    explicit ScheduleManager (const std::vector<ScheduleEntry> &schedule_entries) { this->setSchedule(schedule_entries); }

    static bool validate ();
};


#endif //ALLOC_SCHEDULEMANAGER_H
