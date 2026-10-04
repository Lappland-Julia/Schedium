#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <future>
#include <Schedium.hpp>

#include "Benchmark.hpp"

constexpr int DEFAULT_TEST_VALUE = 1'000;
constexpr int DEFAULT_TESTS = 50;

namespace {
    int _name_1 = 1;
    int _name_2 = 1;

    std::string generate_name_1() {
        return std::to_string(_name_1++);
    }

    std::string generate_name_2() {
        return std::to_string(_name_2++);
    }
}

static std::vector<double> normalize_test(const std::vector<double>& val) {
    std::vector<double> result;

    for (double i : val) {
        result.push_back(i / DEFAULT_TEST_VALUE);
    }

    return result;
}

static std::vector<double> test_create() {
    std::vector<double> result {};

    auto benchmark = Benchmark();

    benchmark.get_start_time();

    std::vector<ScheduleEntry*> data {};
    constexpr std::chrono::sys_seconds start_date =
        std::chrono::sys_days{
            std::chrono::year{2027} / std::chrono::January / 25
        };
    const std::chrono::minutes duration {100};
    for (int i = 0; i < DEFAULT_TEST_VALUE; i++) {
        data.push_back(
            new ScheduleEntry(
                generate_name_1(),
                start_date,
                duration,
                "Test 1",
                "Place 1"
            )
        );
    }

    benchmark.get_end_time();

    result.push_back(benchmark.get_delta_time());

    for (const auto i : data) {
        delete i;
    }

    return result;
}

static std::vector<double> test_validator() {
    std::vector<double> result {};

    auto benchmark = Benchmark();

    std::vector<ScheduleEntry*> data {};

    constexpr std::chrono::sys_seconds start_date =
        std::chrono::sys_days{
            std::chrono::year{2027} / std::chrono::January / 25
        };

    const std::chrono::minutes duration {100};

    for (int i = 0; i < DEFAULT_TEST_VALUE; i++) {
        data.push_back(
            new ScheduleEntry(
                generate_name_2(),
                start_date,
                duration,
                "Test 2",
                "Place 2"
            )
        );
    }

    benchmark.get_start_time();

    auto manager = ScheduleManager();
    for (const auto entry : data) {
        manager.add_schedule_entry(*entry);
    }
    const auto v = Validator(manager);
    bool valid = v.is_valid();

    benchmark.get_end_time();

    result.push_back(benchmark.get_delta_time());

    for (const auto i : data) {
        delete i;
    }

    return result;
}

static std::vector<std::string> output_create_entries() {
    std::vector<std::string> output_result {};
    output_result.push_back("\t\tCREATING "+std::to_string(DEFAULT_TEST_VALUE)+" ENTRIES\n");

    double avg_delta_time = 0;

    for (int i = 0; i < DEFAULT_TESTS; i++) {
        std::vector<double> result = test_create();
        avg_delta_time += result[0];
    }

    avg_delta_time /= DEFAULT_TESTS;

    output_result.push_back("\n\tAverage data:\n");

    output_result.push_back("Average delta time: "+std::to_string(avg_delta_time)+" ms\n");

    const std::vector<double> normalized = normalize_test({
        avg_delta_time
    });

    output_result.push_back("\n\tNormalized per entry:\n");

    output_result.push_back("Delta time: "+std::to_string(normalized[0] * 1000.0)+" us/entry\n");
    return output_result;
}

static std::vector<std::string> output_validate_entries() {
    std::vector<std::string> output_result {};
    output_result.push_back("\n\n\t\tVALIDATING "+std::to_string(DEFAULT_TEST_VALUE)+" ENTRIES\n");

    double validator_avg_delta_time = 0;

    for (int i = 0; i < DEFAULT_TESTS; i++) {
        std::vector<double> result = test_validator();
        validator_avg_delta_time += result[0];
    }

    validator_avg_delta_time /= DEFAULT_TESTS;

    output_result.push_back("\n\tAverage data:\n");

    output_result.push_back("Average delta time: "+std::to_string(validator_avg_delta_time)+" ms\n");

    const std::vector<double> validator_normalized = normalize_test({validator_avg_delta_time});

    output_result.push_back("\n\tNormalized per entry:\n");

    output_result.push_back("Delta time: "+std::to_string(validator_normalized[0] * 1000.0)+" us/entry\n");
    return output_result;
}

static void print_total(const Benchmark& benchmark) {
    const double total_delta_memory =
        benchmark.to_kb(benchmark.get_delta_memo());

    std::cout << "\n\t\tTOTAL\n\n";

    std::cout << "Total time: "
              << benchmark.get_delta_time()
              << " ms\n";

    std::cout << "Total memory: "
              << benchmark.to_mb(benchmark.private_memory())
              << " MB ("
              << (total_delta_memory >= 0 ? "+" : "")
              << total_delta_memory
              << " KB)\n";

    std::cout << "Total peak memory: "
              << benchmark.to_mb(benchmark.peak_private_memory())
              << " MB\n";
}

int main() {
    auto benchmark = Benchmark();


    benchmark.get_start_memo();
    benchmark.get_start_time();

    std::future<std::vector<std::string>> testing_entries = std::async(std::launch::async, output_create_entries);
    std::future<std::vector<std::string>> testing_validator = std::async(std::launch::async, output_validate_entries);
    const std::vector<std::string> result_1 = testing_entries.get();
    const std::vector<std::string> result_2 = testing_validator.get();

    for (const auto& line: result_1) {
        std::cout << line;
    }

    for (const auto& line: result_2) {
        std::cout << line;
    }

    benchmark.get_end_memo();
    benchmark.get_end_time();

    print_total(benchmark);
}
