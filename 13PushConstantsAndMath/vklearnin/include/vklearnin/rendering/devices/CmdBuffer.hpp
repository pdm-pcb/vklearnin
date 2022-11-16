#ifndef VKLEARNIN_RENDERING_DEVICES_CMDBUFFER_HPP
#define VKLEARNIN_RENDERING_DEVICES_CMDBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdBuffer {
public:
    inline const auto & native() const { return _buffer; }

    CmdBuffer() = default;
    ~CmdBuffer() = default;

    CmdBuffer(CmdBuffer &&) = delete;
    CmdBuffer(const CmdBuffer &) = delete;

    CmdBuffer & operator=(CmdBuffer &&) = delete;
    CmdBuffer & operator=(const CmdBuffer &) = delete;

private:
    vk::CommandBuffer _buffer;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DEVICES_CMDBUFFER_HPP