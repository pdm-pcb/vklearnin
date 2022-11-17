#ifndef VKLEARNIN_ENGINE_FRAMEDATA_HPP
#define VKLEARNIN_ENGINE_FRAMEDATA_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/engine/Framebuffer.hpp"
#include "vklearnin/rendering/devices/CmdPool.hpp"
#include "vklearnin/rendering/devices/CmdBuffer.hpp"

namespace vkl {

class FrameData {
public:
    void image_changed();

    void create(const Swapchain &swapchain, const Pipeline &pipeline,
                const uint32_t image_index);
    void destroy();

    inline const auto & framebuffer() const { return _framebuffer; }
    inline const auto & cmd_pool()    const { return _cmd_pool;    }
    inline const auto & cmd_buffer()  const { return _cmd_buffer;  }

    FrameData() = default;
    ~FrameData() = default;

    FrameData(FrameData &&other);
    FrameData(const FrameData &other) = delete;

    FrameData & operator=(FrameData &&other) = delete;
    FrameData & operator=(const FrameData &other) = delete;

private:
    Framebuffer _framebuffer;
    CmdPool     _cmd_pool;
    CmdBuffer   _cmd_buffer;
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_FRAMEDATA_HPP