#include "Benchmark.h"

#include <iostream>
#include <windows.h>
#include <psapi.h>

double Benchmark::private_memory() {
    PROCESS_MEMORY_COUNTERS_EX memory_info{};

    if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memory_info),
        sizeof(memory_info)))
    {
        const double memory_mb = static_cast<double>(memory_info.PrivateUsage); // WorkingSetSize - alt
        return memory_mb;
    }
    return -1;
}

double Benchmark::peak_private_memory() {
    PROCESS_MEMORY_COUNTERS_EX memory_info{};

    if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memory_info),
        sizeof(memory_info)))
    {
        const double memory_mb = static_cast<double>(memory_info.PeakPagefileUsage);
        return memory_mb;
    }
    return -1;
}

void Benchmark::get_start_time() {
    this->start_ = std::chrono::steady_clock::now();
}

void Benchmark::get_end_time() {
    this->end_ = std::chrono::steady_clock::now();
}

double Benchmark::get_delta_time() {
    if (start_ == decltype(start_) {} || end_ == decltype(end_) {}) {
        throw std::invalid_argument("You must use get_start_time() and get_end_time() before this function");
    }
    if (start_ > end_) {
        throw std::invalid_argument("get_start_time() must be used before get_end_time()");
    }
    return std::chrono::duration<double, std::milli>(end_ - start_).count();
}

void Benchmark::get_start_memo() {
    this->start_memo = this->private_memory();
}

void Benchmark::get_end_memo() {
    this->end_memo = this->private_memory();
}

double Benchmark::get_delta_memo() const {
    return this->end_memo - this->start_memo;
}
