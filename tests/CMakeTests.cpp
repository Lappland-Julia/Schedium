#include "Schedium.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <exception>
#include <initializer_list>
#include <iostream>
#include <source_location>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace test {

inline void check(bool condition, const char *expression,
                  std::source_location location = std::source_location::current()) {
    if (!condition) {
        throw std::runtime_error(std::string(location.file_name()) + ":" + std::to_string(location.line()) + ": " +
                                 expression);
    }
}

template <typename Exception, typename Function>
void expect_throws(Function &&function, std::source_location location = std::source_location::current()) {
    try {
        std::forward<Function>(function)();
    } catch (const Exception &) {
        return;
    }
    check(false, "expected exception was not thrown", location);
}

inline ScheduleEntry entry(const std::string &name, int start_minutes = 0, int duration_minutes = 30,
                           const std::string &place = "Room A", const std::string &group = "Group A") {
    const auto day = std::chrono::sys_days{std::chrono::year{2026} / 10 / 11};
    return ScheduleEntry{name, day + std::chrono::minutes{start_minutes}, std::chrono::minutes{duration_minutes}, group,
                         place};
}

using Case = std::pair<const char *, void (*)()>;

inline int run(std::initializer_list<Case> cases, std::size_t expected_case_count) {
    if (cases.size() != expected_case_count) {
        std::cerr << "FAIL: expected " << expected_case_count << " test cases, registered " << cases.size() << '\n';
        return 1;
    }

    int failures = 0;
    for (const auto &[name, function] : cases) {
        try {
            function();
            std::cout << "PASS: " << name << '\n';
        } catch (const std::exception &error) {
            ++failures;
            std::cerr << "FAIL: " << name << ": " << error.what() << '\n';
        } catch (...) {
            ++failures;
            std::cerr << "FAIL: " << name << ": unknown exception\n";
        }
    }
    std::cout << cases.size() - failures << '/' << cases.size() << " cases passed\n";
    return failures == 0 ? 0 : 1;
}

} // namespace test

#define CHECK(expression) ::test::check(static_cast<bool>(expression), #expression)

using namespace std::chrono;

namespace {

void construction_preserves_full_timestamp_and_fields() {
    const auto timestamp = sys_days{year{2026} / 10 / 11} + hours{23} + minutes{59} + seconds{17};
    const ScheduleEntry entry{"Lecture", timestamp, minutes{45}, "Students", "Auditorium"};
    CHECK(entry.get_name() == "Lecture");
    CHECK(entry.get_timestamp() == timestamp);
    CHECK(entry.get_duration() == minutes{45});
    CHECK(entry.get_group_name() == "Students");
    CHECK(entry.get_place_name() == "Auditorium");
    CHECK(!entry.is_owned());
    CHECK(entry.owner_name().empty());
}

void duration_accepts_both_boundaries() {
    auto entry = test::entry("Minimum", 0, 1);
    CHECK(entry.get_duration() == minutes{1});
    entry.set_duration(minutes{6000});
    CHECK(entry.get_duration() == minutes{6000});
    CHECK(test::entry("Maximum", 0, 6000).get_duration() == minutes{6000});
}

void constructor_rejects_invalid_durations() {
    for (const int duration : {-1, 0, 6001}) {
        test::expect_throws<std::invalid_argument>([duration] { test::entry("Invalid", 0, duration); });
    }
}

void failed_duration_update_preserves_entry() {
    auto entry = test::entry("Original", 0, 45);
    for (const int duration : {-1, 0, 6001}) {
        test::expect_throws<std::invalid_argument>([&] { entry.set_duration(minutes{duration}); });
        CHECK(entry.get_duration() == minutes{45});
        CHECK(entry.get_name() == "Original");
    }
}

void setters_update_independent_fields() {
    auto entry = test::entry("Original");
    const auto timestamp = sys_days{year{2027} / 1 / 1} + seconds{5};
    entry.set_name("Renamed");
    entry.set_timestamp(timestamp);
    entry.set_duration(minutes{90});
    entry.set_group_name("New group");
    entry.set_place_name("New room");
    CHECK(entry.get_name() == "Renamed");
    CHECK(entry.get_timestamp() == timestamp);
    CHECK(entry.get_duration() == minutes{90});
    CHECK(entry.get_group_name() == "New group");
    CHECK(entry.get_place_name() == "New room");
}

void ownership_is_copied_without_mutating_original() {
    auto original = test::entry("Lecture");
    original.own("First manager");
    auto copy = original;
    copy.own("Second manager");
    CHECK(original.is_owned());
    CHECK(original.owner_name() == "First manager");
    CHECK(copy.is_owned());
    CHECK(copy.owner_name() == "Second manager");
    CHECK(copy.get_name() == "Lecture");
}

void temporary_string_accessors_return_owned_values() {
    static_assert(std::is_same_v<decltype(std::declval<ScheduleEntry &&>().get_name()), std::string>);
    static_assert(std::is_same_v<decltype(std::declval<const ScheduleEntry &&>().get_name()), std::string>);
    static_assert(std::is_same_v<decltype(std::declval<ScheduleEntry &&>().get_group_name()), std::string>);
    static_assert(std::is_same_v<decltype(std::declval<const ScheduleEntry &&>().get_group_name()), std::string>);
    static_assert(std::is_same_v<decltype(std::declval<ScheduleEntry &&>().get_place_name()), std::string>);
    static_assert(std::is_same_v<decltype(std::declval<const ScheduleEntry &&>().get_place_name()), std::string>);
    static_assert(std::is_same_v<decltype(std::declval<ScheduleEntry &&>().owner_name()), std::string>);
    static_assert(std::is_same_v<decltype(std::declval<const ScheduleEntry &&>().owner_name()), std::string>);
    const auto name = test::entry("Temporary").get_name();
    const auto group = test::entry("Temporary").get_group_name();
    const auto place = test::entry("Temporary").get_place_name();
    CHECK(name == "Temporary");
    CHECK(group == "Group A");
    CHECK(place == "Room A");
}

// ScheduleManager
void default_manager_is_empty() {
    const ScheduleManager manager;
    CHECK(manager.get_name() == "Schedule manager");
    CHECK(manager.get_schedule().empty());
    CHECK(manager.get_schedule_entries().empty());
    CHECK(!manager.name_exists("Missing"));
    test::expect_throws<std::runtime_error>([&] { manager.get_entry("Missing"); });
}

void adding_entry_sets_owner_on_stored_copy() {
    ScheduleManager manager;
    manager.set_name("Faculty");
    const auto original = test::entry("Lecture", 10, 45, "Auditorium", "Students");
    manager.add_schedule_entry(original);
    CHECK(!original.is_owned());
    CHECK(manager.name_exists("Lecture"));
    const auto &stored = manager.get_entry("Lecture");
    CHECK(stored.is_owned());
    CHECK(stored.owner_name() == "Faculty");
    CHECK(stored.get_timestamp() == original.get_timestamp());
    CHECK(stored.get_duration() == std::chrono::minutes{45});
    CHECK(stored.get_group_name() == "Students");
    CHECK(stored.get_place_name() == "Auditorium");
    CHECK(&stored == &manager.get_schedule().front());
    CHECK(&manager.get_schedule() == &manager.get_schedule_entries());
}

void duplicate_add_is_rejected_without_mutation() {
    ScheduleManager manager;
    manager.add_schedule_entry(test::entry("Lecture", 0, 45));
    test::expect_throws<std::runtime_error>([&] { manager.add_schedule_entry(test::entry("Lecture", 60, 10)); });
    CHECK(manager.get_schedule().size() == 1);
    CHECK(manager.get_entry("Lecture").get_duration() == std::chrono::minutes{45});
    CHECK(manager.get_entry("Lecture").get_timestamp() == test::entry("Original").get_timestamp());
}

void vector_constructor_populates_owned_entries() {
    const std::vector entries{test::entry("First"), test::entry("Second", 30)};
    const ScheduleManager manager{entries};
    CHECK(manager.get_schedule().size() == 2);
    CHECK(manager.get_schedule()[0].get_name() == "First");
    CHECK(manager.get_schedule()[1].get_name() == "Second");
    CHECK(manager.get_entry("First").owner_name() == manager.get_name());
    CHECK(manager.get_entry("Second").is_owned());
    CHECK(!entries[0].is_owned());
    test::expect_throws<std::runtime_error>(
        [] { const ScheduleManager duplicate{std::vector{test::entry("Same"), test::entry("Same")}}; });
}

void set_schedule_appends_entries() {
    ScheduleManager manager;
    manager.add_schedule_entry(test::entry("Existing"));
    manager.set_schedule(std::vector{test::entry("First", 30), test::entry("Second", 60)});
    manager.set_schedule(std::vector<ScheduleEntry>{});
    CHECK(manager.get_schedule().size() == 3);
    CHECK(manager.name_exists("Existing"));
    CHECK(manager.name_exists("First"));
    CHECK(manager.name_exists("Second"));
    CHECK(manager.get_entry("Second").is_owned());
}

void field_overload_validates_and_adds_entry() {
    ScheduleManager manager;
    const auto timestamp = test::entry("Clock", 75).get_timestamp();
    manager.set_schedule("Lecture", timestamp, std::chrono::minutes{45}, "Students", "Room B");
    CHECK(manager.get_entry("Lecture").get_timestamp() == timestamp);
    CHECK(manager.get_entry("Lecture").get_place_name() == "Room B");
    CHECK(manager.get_entry("Lecture").get_group_name() == "Students");
    CHECK(manager.get_entry("Lecture").is_owned());
    test::expect_throws<std::invalid_argument>(
        [&] { manager.set_schedule("Invalid", timestamp, std::chrono::minutes{0}, "Students", "Room B"); });
    test::expect_throws<std::runtime_error>(
        [&] { manager.set_schedule("Lecture", timestamp, std::chrono::minutes{30}, "Students", "Room B"); });
    CHECK(manager.get_schedule().size() == 1);
    CHECK(!manager.name_exists("Invalid"));
}

void rename_updates_lookup_and_preserves_fields() {
    ScheduleManager manager{std::vector{test::entry("Old", 75, 45)}};
    manager.rename_schedule_entry("Old", "New");
    CHECK(!manager.name_exists("Old"));
    CHECK(manager.name_exists("New"));
    CHECK(manager.get_entry("New").get_duration() == std::chrono::minutes{45});
    CHECK(manager.get_entry("New").get_timestamp() == test::entry("Clock", 75).get_timestamp());
    CHECK(manager.get_entry("New").owner_name() == manager.get_name());
    test::expect_throws<std::runtime_error>([&] { manager.get_entry("Old"); });
}

void invalid_rename_preserves_schedule() {
    ScheduleManager manager{std::vector{test::entry("First"), test::entry("Second")}};
    test::expect_throws<std::runtime_error>([&] { manager.rename_schedule_entry("First", "Second"); });
    test::expect_throws<std::runtime_error>([&] { manager.rename_schedule_entry("Missing", "New"); });
    test::expect_throws<std::runtime_error>([&] { manager.rename_schedule_entry("Missing", "Missing"); });
    manager.rename_schedule_entry("First", "First");
    CHECK(manager.get_schedule().size() == 2);
    CHECK(manager.get_schedule()[0].get_name() == "First");
    CHECK(manager.get_schedule()[1].get_name() == "Second");
    CHECK(!manager.name_exists("New"));
}

void manager_copy_has_independent_entries() {
    const ScheduleManager original{std::vector{test::entry("Original")}};
    auto copy = original;
    copy.rename_schedule_entry("Original", "Changed");
    copy.add_schedule_entry(test::entry("Extra"));
    CHECK(original.name_exists("Original"));
    CHECK(!original.name_exists("Changed"));
    CHECK(original.get_schedule().size() == 1);
    CHECK(copy.get_schedule().size() == 2);
}

void temporary_schedule_accessors_return_owned_values() {
    static_assert(
        std::is_same_v<decltype(std::declval<ScheduleManager &&>().get_schedule()), std::vector<ScheduleEntry>>);
    static_assert(
        std::is_same_v<decltype(std::declval<const ScheduleManager &&>().get_schedule()), std::vector<ScheduleEntry>>);
    static_assert(std::is_same_v<decltype(std::declval<ScheduleManager &&>().get_schedule_entries()),
                                 std::vector<ScheduleEntry>>);
    static_assert(std::is_same_v<decltype(std::declval<const ScheduleManager &&>().get_schedule_entries()),
                                 std::vector<ScheduleEntry>>);
    const auto schedule = ScheduleManager{std::vector{test::entry("Temporary")}}.get_schedule();
    const auto entries = ScheduleManager{std::vector{test::entry("Temporary")}}.get_schedule_entries();
    CHECK(schedule.size() == 1);
    CHECK(schedule.front().get_name() == "Temporary");
    CHECK(entries.size() == 1);
    CHECK(entries.front().is_owned());
}

// Validator
void empty_schedules_are_valid() {
    const Validator empty;
    const Validator empty_manager{ScheduleManager{}};
    const Validator empty_managers{std::vector<ScheduleManager>{}};
    CHECK(empty.is_valid());
    CHECK(empty.score() == 0);
    CHECK(empty_manager.is_valid());
    CHECK(empty_manager.score() == 0);
    CHECK(empty_managers.is_valid());
    CHECK(empty_managers.score() == 0);
}

void validating_levels_accept_only_known_names() {
    Validator validator;
    CHECK(validator.get_validating_level() == "MEDIUM");
    for (const std::string level : {"EASY", "MEDIUM", "HARD"}) {
        validator.set_validating_level(level);
        CHECK(validator.get_validating_level() == level);
    }
    for (const std::string level : {"", "easy", "UNKNOWN"}) {
        test::expect_throws<std::invalid_argument>([&] { validator.set_validating_level(level); });
        CHECK(validator.get_validating_level() == "HARD");
    }
}

void gap_score_can_be_updated() {
    Validator validator;
    CHECK(validator.get_gap_score() == 5);
    validator.update_gap_score(17);
    CHECK(validator.get_gap_score() == 17);
    validator.update_gap_score(0);
    CHECK(validator.get_gap_score() == 0);
}

void one_entry_has_no_conflicts() {
    const Validator validator{ScheduleManager{std::vector{test::entry("Only")}}};
    CHECK(validator.is_valid());
    CHECK(validator.score() == 0);
}

void touching_and_separated_entries_are_valid_in_any_order() {
    std::vector entries{test::entry("First", 0), test::entry("Touching", 30), test::entry("Later", 120)};
    for (int order = 0; order < 3; ++order) {
        const Validator validator{ScheduleManager{entries}};
        CHECK(validator.is_valid());
        CHECK(validator.score() == 0);
        std::rotate(entries.begin(), entries.begin() + 1, entries.end());
    }
}

void overlapping_entries_have_a_penalty_in_either_order() {
    std::vector entries{test::entry("First", 0, 45), test::entry("Second", 30, 45)};
    for (int order = 0; order < 2; ++order) {
        const Validator validator{ScheduleManager{entries}};
        CHECK(!validator.is_valid());
        CHECK(validator.score() == 10000);
        std::reverse(entries.begin(), entries.end());
    }
}

void identical_start_times_conflict() {
    const Validator validator{ScheduleManager{std::vector{test::entry("First"), test::entry("Second", 0, 1)}}};
    CHECK(!validator.is_valid());
    CHECK(validator.score() == 10000);
}

void nested_intervals_conflict() {
    const Validator validator{ScheduleManager{
        std::vector{test::entry("Outer", 0, 180), test::entry("Inner", 30, 15), test::entry("Later", 90, 15)}}};
    CHECK(!validator.is_valid());
}

void simultaneous_entries_in_different_rooms_are_valid() {
    const Validator validator{
        ScheduleManager{std::vector{test::entry("First", 0, 45, "Room A"), test::entry("Second", 0, 45, "Room B")}}};
    CHECK(validator.is_valid());
    CHECK(validator.score() == 0);
}

void room_conflicts_are_detected_across_managers_and_groups() {
    const ScheduleManager first{std::vector{test::entry("First", 0, 45, "Room A", "Group A")}};
    const ScheduleManager second{std::vector{test::entry("Second", 30, 30, "Room A", "Group B")}};
    const Validator constructed{std::vector{first, second}};
    Validator appended;
    appended.add_manager(first);
    appended.add_managers(std::vector{second});
    CHECK(!constructed.is_valid());
    CHECK(constructed.score() == 10000);
    CHECK(!appended.is_valid());
    CHECK(appended.score() == 10000);
}

void different_dates_do_not_conflict_at_same_clock_time() {
    const Validator validator{
        ScheduleManager{std::vector{test::entry("Today", 600, 45), test::entry("Tomorrow", 2040, 45)}}};
    CHECK(validator.is_valid());
    CHECK(validator.score() == 0);
}

void overnight_intervals_conflict_with_the_next_day() {
    const Validator overlap{
        ScheduleManager{std::vector{test::entry("Overnight", 1430, 30), test::entry("Next day", 1445, 30)}}};
    const Validator touching{
        ScheduleManager{std::vector{test::entry("Overnight", 1430, 30), test::entry("Next day", 1460, 30)}}};
    CHECK(!overlap.is_valid());
    CHECK(overlap.score() == 10000);
    CHECK(touching.is_valid());
    CHECK(touching.score() == 0);
}

void seconds_are_retained_when_checking_endpoints() {
    auto first = test::entry("First", 0, 1);
    first.set_timestamp(first.get_timestamp() + std::chrono::seconds{30});
    auto second = test::entry("Second", 1, 1);
    second.set_timestamp(second.get_timestamp() + std::chrono::seconds{29});
    const Validator overlap{ScheduleManager{std::vector{first, second}}};
    CHECK(!overlap.is_valid());
    CHECK(overlap.score() == 10000);
    second.set_timestamp(second.get_timestamp() + std::chrono::seconds{1});
    const Validator touching{ScheduleManager{std::vector{first, second}}};
    CHECK(touching.is_valid());
    CHECK(touching.score() == 0);
}

void penalties_accumulate_across_independent_conflicts() {
    const Validator validator{
        ScheduleManager{std::vector{test::entry("A1", 0, 45, "Room A"), test::entry("A2", 30, 45, "Room A"),
                                    test::entry("B1", 0, 45, "Room B"), test::entry("B2", 30, 45, "Room B"),
                                    test::entry("A3", 120, 45, "Room A"), test::entry("A4", 150, 45, "Room A")}}};
    CHECK(!validator.is_valid());
    CHECK(validator.score() == 30000);
    CHECK(validator.score() == 30000);
}

void validator_keeps_a_copy_of_added_managers() {
    ScheduleManager manager{std::vector{test::entry("Original")}};
    const Validator snapshot{manager};
    manager.add_schedule_entry(test::entry("Conflict"));
    CHECK(snapshot.is_valid());
    CHECK(snapshot.score() == 0);
    CHECK(!Validator{manager}.is_valid());
}

} // namespace

int main() {
    return test::run(
        {
            {"ScheduleEntry / construction preserves timestamp and fields",
             construction_preserves_full_timestamp_and_fields},
            {"ScheduleEntry / duration accepts both boundaries", duration_accepts_both_boundaries},
            {"ScheduleEntry / constructor rejects invalid durations", constructor_rejects_invalid_durations},
            {"ScheduleEntry / failed duration update preserves entry", failed_duration_update_preserves_entry},
            {"ScheduleEntry / setters update independent fields", setters_update_independent_fields},
            {"ScheduleEntry / ownership is copied independently", ownership_is_copied_without_mutating_original},
            {"ScheduleEntry / temporary accessors return owned values", temporary_string_accessors_return_owned_values},
            {"ScheduleManager / default manager is empty", default_manager_is_empty},
            {"ScheduleManager / adding entry sets owner on stored copy", adding_entry_sets_owner_on_stored_copy},
            {"ScheduleManager / duplicate add preserves schedule", duplicate_add_is_rejected_without_mutation},
            {"ScheduleManager / vector constructor populates owned entries",
             vector_constructor_populates_owned_entries},
            {"ScheduleManager / set_schedule appends entries", set_schedule_appends_entries},
            {"ScheduleManager / field overload validates and adds entry", field_overload_validates_and_adds_entry},
            {"ScheduleManager / rename updates lookup and preserves fields",
             rename_updates_lookup_and_preserves_fields},
            {"ScheduleManager / invalid rename preserves schedule", invalid_rename_preserves_schedule},
            {"ScheduleManager / manager copy has independent entries", manager_copy_has_independent_entries},
            {"ScheduleManager / temporary schedule accessors return owned values",
             temporary_schedule_accessors_return_owned_values},
            {"Validator / empty schedules are valid", empty_schedules_are_valid},
            {"Validator / validating levels accept only known names", validating_levels_accept_only_known_names},
            {"Validator / gap score can be updated", gap_score_can_be_updated},
            {"Validator / one entry has no conflicts", one_entry_has_no_conflicts},
            {"Validator / touching and separated entries are valid",
             touching_and_separated_entries_are_valid_in_any_order},
            {"Validator / overlapping entries have a penalty", overlapping_entries_have_a_penalty_in_either_order},
            {"Validator / identical start times conflict", identical_start_times_conflict},
            {"Validator / nested intervals conflict", nested_intervals_conflict},
            {"Validator / different rooms do not conflict", simultaneous_entries_in_different_rooms_are_valid},
            {"Validator / conflicts span managers and groups", room_conflicts_are_detected_across_managers_and_groups},
            {"Validator / different dates do not conflict", different_dates_do_not_conflict_at_same_clock_time},
            {"Validator / overnight intervals use the next day", overnight_intervals_conflict_with_the_next_day},
            {"Validator / seconds are retained at endpoints", seconds_are_retained_when_checking_endpoints},
            {"Validator / penalties accumulate independently", penalties_accumulate_across_independent_conflicts},
            {"Validator / validator keeps a manager snapshot", validator_keeps_a_copy_of_added_managers},
        },
        32);
}
