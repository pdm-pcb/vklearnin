#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/Timekeeper.hpp"

namespace vkl {

Timekeeper::HRC::time_point Timekeeper::_app_start = Timekeeper::HRC::now();

Timekeeper::HRC::time_point Timekeeper::_frame_start   = Timekeeper::_app_start;
Timekeeper::HRC::time_point Timekeeper::_present_start = Timekeeper::_app_start;

uint64_t Timekeeper::_run_time     = 0u;
uint64_t Timekeeper::_frame_time   = 0u;
uint64_t Timekeeper::_present_time = 0u;

void Timekeeper::update() {
    auto interval = HRC::now() - _app_start;
    _run_time = std::chrono::duration_cast<Microseconds>(interval).count();
}

void Timekeeper::frame_start() {
    _frame_start = HRC::now();
}

void Timekeeper::frame_end() {
    auto interval = HRC::now() - _frame_start;
    _frame_time = std::chrono::duration_cast<Microseconds>(interval).count();
}

void Timekeeper::present_start() {
    _present_start = HRC::now();
}

void Timekeeper::present_end() {
    auto interval = HRC::now() - _present_start;
    _present_time = std::chrono::duration_cast<Microseconds>(interval).count();
}

} // namespace vkl