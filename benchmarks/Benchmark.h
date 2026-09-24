#ifndef SCHEDIUM_VALIDATORCHECKER_H
#define SCHEDIUM_VALIDATORCHECKER_H

#include <functional>
#include <chrono>



class Benchmark {

public:
    static double private_memory();
    static double peak_private_memory();

    template <typename F>
    static double time_check(F function) {
        const auto before = std::chrono::steady_clock::now();
        function();
        const auto after = std::chrono::steady_clock::now();
        const std::chrono::duration<double, std::milli> duration = after - before;
        return duration.count();
    }
};


#endif //SCHEDIUM_VALIDATORCHECKER_H
