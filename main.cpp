#include <iostream>
#include <vector>
#include <windows.h>
#include "src/ScheduleEntry.h"
#include "src/ScheduleManager.h"
#include "src/Generators.h"
#include "src/Validator.h"
#include "benchmarks/Benchmark.h"

void create_pool() {
    const std::chrono::sys_seconds date1 = std::chrono::sys_days{std::chrono::year{2027}/std::chrono::January/25};
    const std::chrono::minutes time1 {100};
    auto b = Benchmark();
    std::cout << "Memory usage (before): " << b.private_memory() << " KB\n";
    std::vector<ScheduleEntry*> values {};
    for (int i = 0; i < 10000; i++) {
        values.push_back(new ScheduleEntry("Memory Test", date1, time1, 10, 1));
    }
    std::cout << "Memory usage (after): " << b.private_memory() << " KB\n";
    for (auto elem: values) {
        delete elem;
    }
    std::cout << "Memory usage (delete): " << b.private_memory() << " KB\n";
    std::cout << "Peak memory usage: " << b.peak_private_memory() << " KB\n";
}

int main() {

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "Schedule Creator\n";

    std::chrono::sys_seconds date1 = std::chrono::sys_days{std::chrono::year{2027}/std::chrono::January/25};
    std::chrono::minutes time1 {100};

    auto first_lesson = ScheduleEntry("Lesson 1", date1, time1, 10, 1);
    auto second_lesson = ScheduleEntry("Lesson 2", date1, time1, 10, 1);
    auto third_lesson = ScheduleEntry("Lesson 3", date1, time1, 10, 1);

    first_lesson.set_id(52);

    auto manager = ScheduleManager();
    manager.add_schedule_entry(first_lesson);
    manager.add_schedule_entry(second_lesson);
    manager.add_schedule_entry(third_lesson);

     std::cout << "Group ID: " << first_lesson.get_id() << "; Group Name: " << first_lesson.get_name() << "\n";
     std::cout << "ID 52 match: " << manager.id_exists(52) << "; ID 53 match: " << manager.id_exists(53) << "\n";
     std::cout << "Lesson 1 match: " << manager.name_exists("Lesson 1") << "; Invalid name match: " << manager.name_exists("Invalid name") << "\n";
     std::cout << "Lesson 1 owner (by id): " << manager.get_entry_by_id(52).get_id() << "\n";
     std::cout << "Lesson 2 owner (by name): " << manager.get_entry_by_name("Lesson 2").get_id() << "\n";
     std::cout << "Lesson 3 owner (by name): " << manager.get_entry_by_name("Lesson 3").get_id() << "\n";
     std::cout << "Random ID: " << generate_id() << "\n\n";
     std::cout << "SCHEDULE ENTRY:\n";
     std::cout << "Manager ID: " << std::to_string(manager.get_id()) << "\n\nLessons: \n";
     for (ScheduleEntry entry: manager.get_schedule()) {
         std::cout << "ID: " << std::to_string(entry.get_id()) << "; Name: " << entry.get_name() << "\n";
     }
    std::cout << "\nVALIDATORS:\n";
    auto val = Validator();
    val.add_manager(manager);
    val.set_validating_level("HARD");
    std::cout << "Validating power: " << val.get_validating_level() << "\n";

    std::cout << "\nValid schedule: "<< val.is_valid() <<"\n";

    return 0;
}