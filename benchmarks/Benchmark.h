#ifndef SCHEDIUM_VALIDATORCHECKER_H
#define SCHEDIUM_VALIDATORCHECKER_H

#include <functional>
#include <chrono>



class Benchmark {
private:
    std::chrono::steady_clock::time_point start_;
    std::chrono::steady_clock::time_point end_;

    double start_memo = -1;
    double end_memo = -1;

public:
    static double private_memory();
    static double peak_private_memory();

    void get_start_time();
    void get_end_time();
    double get_delta_time();

    void get_start_memo();
    void get_end_memo();
    double get_delta_memo() const;

    static double to_kb(const double val) { return val / 1024.0; }
    static double to_mb(const double val) { return val / 1024.0 / 1024.0; }
};


#endif //SCHEDIUM_VALIDATORCHECKER_H
