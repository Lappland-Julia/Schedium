#include <iostream>
#include <vector>
#include <string>
#include <Schedium.h>

#include "Benchmark.h"

constexpr int DEFAULT_TEST_VALUE = 10'000;
constexpr int DEFAULT_TESTS = 10;

static std::vector<double> test() {
    std::vector<double> result {};

    auto benchmark = Benchmark();
    result.push_back(benchmark.to_mb(benchmark.private_memory()));

    benchmark.get_start_time();
    benchmark.get_start_memo();

    std::vector<ScheduleEntry*> data {};

    constexpr std::chrono::sys_seconds start_date =
        std::chrono::sys_days{
            std::chrono::year{2027} / std::chrono::January / 25
        };

    const std::chrono::minutes duration {100};

    for (int i = 0; i < DEFAULT_TEST_VALUE; i++) {
        data.push_back(
            new ScheduleEntry(
                std::to_string(generate_id()),
                start_date,
                duration,
                0,
                0
            )
        );
    }

    benchmark.get_end_time();
    benchmark.get_end_memo();

    result.push_back(benchmark.get_delta_time());
    result.push_back(benchmark.to_mb(benchmark.private_memory()));
    result.push_back(benchmark.to_kb(benchmark.get_delta_memo()));

    for (auto i : data) {
        delete i;
    }

    return result;
}

static std::vector<double> normalize_test(const std::vector<double>& val) {
    std::vector<double> result;

    for (double i : val) {
        result.push_back(i / DEFAULT_TEST_VALUE);
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
                std::to_string(generate_id()),
                start_date,
                duration,
                0,
                0
            )
        );
    }

    result.push_back(benchmark.to_mb(benchmark.private_memory()));

    benchmark.get_start_time();
    benchmark.get_start_memo();

    auto manager = ScheduleManager();

    for (auto i : data) {
        manager.add_schedule_entry(*i);
    }

    auto v = Validator(manager);
    bool valid = v.is_valid();

    benchmark.get_end_time();
    benchmark.get_end_memo();

    result.push_back(benchmark.get_delta_time());
    result.push_back(benchmark.to_mb(benchmark.private_memory()));
    result.push_back(benchmark.to_kb(benchmark.get_delta_memo()));

    for (auto i : data) {
        delete i;
    }

    return result;
}

int main() {
    std::cout << "\t\tCREATING "
              << DEFAULT_TEST_VALUE
              << " ENTRIES\n";

    auto benchmark = Benchmark();

    benchmark.get_start_memo();
    benchmark.get_start_time();

    double avg_start_memo = 0;
    double avg_delta_time = 0;
    double avg_end_memo = 0;
    double avg_delta_memo = 0;

    for (int i = 0; i < DEFAULT_TESTS; i++) {
        std::vector<double> result = test();

        avg_start_memo += result[0];
        avg_delta_time += result[1];
        avg_end_memo += result[2];
        avg_delta_memo += result[3];
    }

    avg_start_memo /= DEFAULT_TESTS;
    avg_delta_time /= DEFAULT_TESTS;
    avg_end_memo /= DEFAULT_TESTS;
    avg_delta_memo /= DEFAULT_TESTS;

    std::cout << "\n\tAverage data:\n";

    std::cout << "Average memory: "
              << avg_start_memo
              << " MB ("
              << (avg_delta_memo >= 0 ? "+" : "")
              << avg_delta_memo
              << " KB)\n";

    std::cout << "Average delta time: "
              << avg_delta_time
              << " ms\n";

    const std::vector<double> normalized = normalize_test({
        avg_delta_time
    });

    std::cout << "\n\tNormalized per entry:\n";

    std::cout << "Delta time: "
              << normalized[0] * 1000.0
              << " us/entry\n";


    std::cout << "\n\n\t\tVALIDATING "
              << DEFAULT_TEST_VALUE
              << " ENTRIES\n";

    double validator_avg_start_memo = 0;
    double validator_avg_delta_time = 0;
    double validator_avg_end_memo = 0;
    double validator_avg_delta_memo = 0;

    for (int i = 0; i < DEFAULT_TESTS; i++) {
        std::vector<double> result = test_validator();

        validator_avg_start_memo += result[0];
        validator_avg_delta_time += result[1];
        validator_avg_end_memo += result[2];
        validator_avg_delta_memo += result[3];
    }

    validator_avg_start_memo /= DEFAULT_TESTS;
    validator_avg_delta_time /= DEFAULT_TESTS;
    validator_avg_end_memo /= DEFAULT_TESTS;
    validator_avg_delta_memo /= DEFAULT_TESTS;

    std::cout << "\n\tAverage data:\n";

    std::cout << "Average memory: "
              << validator_avg_start_memo
              << " MB ("
              << (validator_avg_delta_memo >= 0 ? "+" : "")
              << validator_avg_delta_memo
              << " KB)\n";

    std::cout << "Average delta time: "
              << validator_avg_delta_time
              << " ms\n";

    const std::vector<double> validator_normalized = normalize_test({validator_avg_delta_time});

    std::cout << "\n\tNormalized per entry:\n";

    std::cout << "Delta time: "
              << validator_normalized[0] * 1000.0
              << " us/entry\n";

    benchmark.get_end_memo();
    benchmark.get_end_time();

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