#include "Benchmark.h"

#include <iostream>
#include <windows.h>
#include <psapi.h>

double Benchmark::private_memory() {
    PROCESS_MEMORY_COUNTERS_EX memory_info{};

    if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memory_info),
        sizeof(memory_info)))
    {
        const double memory_mb = static_cast<double>(memory_info.PrivateUsage) / 1024.0; // WorkingSetSize - alt
        return memory_mb;
    }
    return -1;
}

double Benchmark::peak_private_memory() {
    PROCESS_MEMORY_COUNTERS_EX memory_info{};

    if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memory_info),
        sizeof(memory_info)))
    {
        const double memory_mb = static_cast<double>(memory_info.PeakPagefileUsage) / 1024.0;
        return memory_mb;
    }
    return -1;
}
