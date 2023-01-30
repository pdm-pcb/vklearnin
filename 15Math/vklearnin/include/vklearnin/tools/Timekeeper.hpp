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

    inline static auto runtime()   { return 1e-6f * static_cast<float>(_runtime);   }
    inline static auto frametime() { return 1e-6f * static_cast<float>(_frametime); }

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

    static uint64_t _runtime;
    static uint64_t _frametime;
};

} // namespace vkl

#endif // VKLEARNIN_TOOLS_TIMEKEEPER_HPP