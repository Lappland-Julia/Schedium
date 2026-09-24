#include <iostream>
#include <vector>
#include <Schedium.h>

#include "Benchmark.h"

constexpr int DEFAULT_TEST_VALUE = 10'000;
constexpr int DEFAULT_TESTS = 10;

std::vector<double> test() {
    std::vector<double> result {};

    auto benchmark = Benchmark();
    result.push_back(benchmark.to_mb(benchmark.private_memory()));

    benchmark.get_start_time();
    benchmark.get_start_memo();
    std::vector<ScheduleEntry*> data {};

    constexpr std::chrono::sys_seconds start_date = std::chrono::sys_days{std::chrono::year{2027}/std::chrono::January/25};
    const std::chrono::minutes duration {100};

    for (int i = 0; i < DEFAULT_TEST_VALUE; i++) {
        data.push_back(new ScheduleEntry("Test", start_date, duration, 0, 0));
    }

    benchmark.get_end_time();
    benchmark.get_end_memo();

    result.push_back(benchmark.get_delta_time());
    result.push_back(benchmark.to_mb(benchmark.private_memory()));
    result.push_back(benchmark.to_kb(benchmark.get_delta_memo()));

    for (auto i: data) {
        delete i;
    }

    return result;
}

std::vector<double> normalize_test(const std::vector<double>& val) {
    std::vector<double> result;
    for (double i : val) {
        result.push_back(i / DEFAULT_TEST_VALUE);
    }

    return result;
}

int main() {

    std::cout << "\t\tCREATING " << DEFAULT_TEST_VALUE << " ENTRIES\n";

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
    std::cout << "Average start memory: " << avg_start_memo << " MB\n";
    std::cout << "Average end memory: " << avg_end_memo << " MB\n";
    std::cout << "Average delta memory: " << avg_delta_memo << " KB\n";
    std::cout << "Average delta time: " << avg_delta_time << " ms\n";

    const std::vector<double> normalized = normalize_test({avg_delta_memo});

    std::cout << "\n\tNormalized per entry:\n";
    std::cout << "Delta time: " << normalized[0] * 1000.0 << " us/entry\n";

    // continue here

    benchmark.get_end_memo();
    benchmark.get_end_time();

    std::cout << "\n\tTotal parameters:\n";
    std::cout << "Total time: " << benchmark.get_delta_time() << " ms\n";
    std::cout << "Total end memory: " << benchmark.to_mb(benchmark.private_memory()) << " MB\n";
    std::cout << "Total peak memory: " << benchmark.to_mb(benchmark.peak_private_memory()) << " MB\n";
    std::cout << "Total delta memory: " << benchmark.to_kb(benchmark.get_delta_memo()) << " KB\n";
}
