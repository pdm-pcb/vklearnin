#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/FrameData.hpp"

namespace vkl {

void FrameData::create() {
    _cmd_pool.create();
    _cmd_buffer.create(_cmd_pool);
}

void FrameData::destroy() {
    _cmd_buffer.destroy();
    _cmd_pool.destroy();
}

FrameData::FrameData(FrameData &&other) :
    _cmd_pool    { std::move(other._cmd_pool)    },
    _cmd_buffer  { std::move(other._cmd_buffer)  }
{ }

} // namespace vkl