#ifndef ALLOC_SCHEDULE_H
#define ALLOC_SCHEDULE_H
#include <chrono>
#include <string>


class ScheduleEntry {
private:
    int id = 0;
    std::string name;
    std::chrono::seconds duration;
    std::chrono::sys_seconds timestamp;
    int groupId = 0;
    int placeId = 0;

public:

    int getId() const { return this->id; }
    void setId(const int &new_id);

    std::string getName() const { return this->name; }
    void setName(const std::string &new_name);

    std::chrono::sys_seconds getTimestamp() const { return this->timestamp; }
    void setTimestamp(const std::chrono::sys_seconds &new_timestamp);

    std::chrono::seconds getDuration() const { return this->duration; };
    void setDuration(const std::chrono::seconds &new_duration);

    int getGroupId() const { return this->groupId; }
    void setGroupId(const int &new_group_id);

    int getPlaceId() const { return this->placeId; }
    void setPlaceId(const int &new_place_id);

    ScheduleEntry (const std::string &name, const std::chrono::sys_seconds &timestamp, const std::chrono::seconds &duration, const int &group_id, const int &place_id);

};


#endif //ALLOC_SCHEDULE_H
