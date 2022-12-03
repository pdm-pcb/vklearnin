#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/Timekeeper.hpp"

namespace vkl {

Timekeeper::HRC::time_point Timekeeper::_app_start   = Timekeeper::HRC::now();
Timekeeper::HRC::time_point Timekeeper::_frame_start = Timekeeper::_app_start;
uint64_t Timekeeper::_runtime   = 0u;
uint64_t Timekeeper::_frametime = 0u;

void Timekeeper::frame_start() {
    _frame_start = HRC::now();
}

void Timekeeper::frame_end() {
    auto interval = HRC::now() - _frame_start;
    _frametime = std::chrono::duration_cast<Microseconds>(interval).count();
}

void Timekeeper::update() {
    auto interval = HRC::now() - _app_start;
    _runtime = std::chrono::duration_cast<Microseconds>(interval).count();
}

} // namespace vkl