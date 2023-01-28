#ifndef VKLEARNIN_RENDER_RENDERPASS_CMDBUFFER_HPP
#define VKLEARNIN_RENDER_RENDERPASS_CMDBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdBuffer final {
public:
    void allocate(const vk::CommandBufferAllocateInfo &buffer_info);

    inline const auto & native() const { return _buffer; }

    CmdBuffer();
    ~CmdBuffer() = default;

    CmdBuffer(CmdBuffer &&other) noexcept;
    CmdBuffer(const CmdBuffer &) = delete;

    CmdBuffer & operator=(CmdBuffer &&) = delete;
    CmdBuffer & operator=(const CmdBuffer &) = delete;

private:
    vk::CommandBuffer _buffer;
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_RENDERPASS_CMDBUFFER_HPP