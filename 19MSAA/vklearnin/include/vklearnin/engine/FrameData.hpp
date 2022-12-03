#ifndef VKLEARNIN_ENGINE_FRAMEDATA_HPP
#define VKLEARNIN_ENGINE_FRAMEDATA_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/devices/CmdPool.hpp"
#include "vklearnin/rendering/devices/CmdBuffer.hpp"

namespace vkl {

class FrameData final {
public:
    void create();
    void destroy();

    inline const auto & command_pool()   const { return _command_pool; }
    inline const auto & command_buffer() const { return _command_buffer; }

    FrameData() = default;
    ~FrameData() = default;

    FrameData(FrameData &&other);
    FrameData(const FrameData &other) = delete;

    FrameData & operator=(FrameData &&other) = delete;
    FrameData & operator=(const FrameData &other) = delete;

private:
    CmdPool   _command_pool;
    CmdBuffer _command_buffer;
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_FRAMEDATA_HPP