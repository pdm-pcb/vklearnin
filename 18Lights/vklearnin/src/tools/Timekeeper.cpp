#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/Timekeeper.hpp"

namespace vkl {

Timekeeper::HRC::time_point Timekeeper::_tick_start = Timekeeper::HRC::now();

uint64_t Timekeeper::_run_time     = 0u;
uint64_t Timekeeper::_tick_delta   = 0u;

void Timekeeper::update() {
    auto const now = HRC::now();
    auto const delta = now - _tick_start;

    _tick_start = now;
    _tick_delta = std::chrono::duration_cast<Microseconds>(delta).count();

    _run_time += _tick_delta;
}

} // namespace vkl