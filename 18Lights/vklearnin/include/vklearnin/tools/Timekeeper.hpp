#ifndef VKLEARNIN_TOOLS_TIMEKEEPER_HPP
#define VKLEARNIN_TOOLS_TIMEKEEPER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Timekeeper final {
public:
    using HRC = std::chrono::high_resolution_clock;
    using Microseconds = std::chrono::microseconds;

    static void update();

    static void frame_start();
    static void frame_end();

    static void present_start();
    static void present_end();

    inline static auto run_time() {
        return 1e-6f * static_cast<float>(_run_time);
    }

    inline static auto frame_time() {
        return 1e-6f * static_cast<float>(_frame_time);
    }

    inline static auto present_time() {
        return 1e-6f * static_cast<float>(_present_time);
    }

    inline static auto now() { return HRC::now(); }

    Timekeeper() = delete;
    ~Timekeeper() = delete;

    Timekeeper(Timekeeper &&other) = delete;
    Timekeeper(const Timekeeper &other) = delete;

    Timekeeper& operator=(Timekeeper &&other) = delete;
    Timekeeper& operator=(const Timekeeper &other) = delete;

private:
    static HRC::time_point _app_start;
    static HRC::time_point _frame_start;
    static HRC::time_point _present_start;

    static uint64_t _run_time;
    static uint64_t _frame_time;
    static uint64_t _present_time;
};

} // namespace vkl

#endif // VKLEARNIN_TOOLS_TIMEKEEPER_HPP