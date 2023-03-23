#ifndef VKLEARNIN_TOOLS_TIMEKEEPER_HPP
#define VKLEARNIN_TOOLS_TIMEKEEPER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Timekeeper final {
public:
    using HRC = std::chrono::high_resolution_clock;
    using Microseconds = std::chrono::microseconds;

    static void update();

    inline static auto run_time() {
        return 1e-6f * static_cast<float>(_run_time);
    }

    inline static auto tick_delta() {
        return 1e-6f * static_cast<float>(_tick_delta);
    }

    inline static auto now() { return HRC::now(); }

    Timekeeper() = delete;

private:
    static HRC::time_point _tick_start;

    static uint64_t _run_time;
    static uint64_t _tick_delta;
};

} // namespace vkl

#endif // VKLEARNIN_TOOLS_TIMEKEEPER_HPP