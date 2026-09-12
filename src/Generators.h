#ifndef SCHEDIUM_GENERATORS_H
#define SCHEDIUM_GENERATORS_H
#include <format>
#include <chrono>
#include <random>

inline int generateId() {
    return static_cast<int>(std::hash<std::string>{}(std::format("{:%Y-%m-%d %H:%M:%S}-{}", std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()), std::random_device{}())) & 0x7FFFFFFF);
}

#endif //SCHEDIUM_GENERATORS_H
