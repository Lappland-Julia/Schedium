#ifndef ALLOC_SCHEDULEMANAGER_H
#define ALLOC_SCHEDULEMANAGER_H
#include <vector>
#include "ScheduleEntry.h"


class ScheduleManager {
private:
    std::string name = "Schedule manager";
    std::vector<ScheduleEntry> scheduleVec;

public:
    std::string getName() { return this->name; }
    void setName(const std::string &new_name) { this->name = new_name; }

    std::vector<ScheduleEntry> getSchedule() const { return this->scheduleVec; }

    void addScheduleEntry(const ScheduleEntry &new_schedule_entry);
    void setSchedule(const std::vector<ScheduleEntry> &new_schedule_vec) { this->scheduleVec = new_schedule_vec; }

    bool idExists(const int &target_id) const;
    bool nameExists(const std::string &target_name) const;

    ScheduleManager () { this->scheduleVec = std::vector<ScheduleEntry>{}; }
    explicit ScheduleManager (const std::vector<ScheduleEntry> &schedule_entries) { this->setSchedule(schedule_entries); }
};


#endif //ALLOC_SCHEDULEMANAGER_H
