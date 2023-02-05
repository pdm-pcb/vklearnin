#ifndef VKLEARNIN_SYSTEM_DEVICES_CMDBUFFER_HPP
#define VKLEARNIN_SYSTEM_DEVICES_CMDBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdBuffer final {
public:
    void allocate(const vk::CommandPool pool, const bool primary = true);
    void free();

    inline auto const& native() const { return _buffer; }

    CmdBuffer();
    ~CmdBuffer() = default;

    CmdBuffer(CmdBuffer &&other) noexcept;
    CmdBuffer(const CmdBuffer &) = delete;

    CmdBuffer& operator=(CmdBuffer &&) = delete;
    CmdBuffer& operator=(const CmdBuffer &) = delete;

private:
    vk::CommandPool   _pool;
    vk::CommandBuffer _buffer;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_DEVICES_CMDBUFFER_HPP