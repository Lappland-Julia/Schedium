#include <iostream>
#include <vector>
#include <windows.h>
#include "src/ScheduleEntry.h"
#include "src/ScheduleManager.h"
#include "src/Generators.h"

int main() {

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "Schedule Creator\n";

    std::chrono::sys_seconds date1 = std::chrono::sys_days{std::chrono::year{2027}/std::chrono::January/25};
    std::chrono::seconds time1 {4800};

    auto first_lesson = ScheduleEntry("Lesson 1", date1, time1, 10, 1);
    auto second_lesson = ScheduleEntry("Lesson 2", date1, time1, 10, 1);
    auto third_lesson = ScheduleEntry("Lesson 3", date1, time1, 10, 1);

    first_lesson.setId(52);

    auto manager = ScheduleManager();
    manager.addScheduleEntry(first_lesson);
    manager.addScheduleEntry(second_lesson);
    manager.addScheduleEntry(third_lesson);

    std::cout << "Group ID: " << first_lesson.getId() << "; Group Name: " << first_lesson.getName() << "\n";
    std::cout << "ID 52 match: " << manager.idExists(52) << "; ID 53 match: " << manager.idExists(53) << "\n";
    std::cout << "Lesson 1 match: " << manager.nameExists("Lesson 1") << "; Invalid name match: " << manager.nameExists("Invalid name") << "\n";
    std::cout << "Lesson 1 owner (by id): " << manager.getEntryById(52).getId() << "\n";
    std::cout << "Lesson 2 owner (by name): " << manager.getEntryByName("Lesson 2").getId() << "\n";
    std::cout << "Lesson 3 owner (by name): " << manager.getEntryByName("Lesson 3").getId() << "\n";
    std::cout << "Random ID: " << generateId() << "\n\n";
    std::cout << "SCHEDULE ENTRY:\n";
    std::cout << "Manager ID: " << std::to_string(manager.getId()) << "\n\nLessons: \n";
    for (ScheduleEntry entry: manager.getSchedule()) {
        std::cout << "ID: " << std::to_string(entry.getId()) << "; Name: " << entry.getName() << "\n";
    }

    return 0;
}