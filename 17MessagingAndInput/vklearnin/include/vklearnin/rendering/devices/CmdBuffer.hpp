#ifndef VKLEARNIN_RENDERING_DEVICES_CMDBUFFER_HPP
#define VKLEARNIN_RENDERING_DEVICES_CMDBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdPool;

class CmdBuffer final {
public:
    void create(const CmdPool &pool);
    void destroy();

    inline const auto & native() const { return _buffer; }

    CmdBuffer() = default;
    ~CmdBuffer() = default;

    CmdBuffer(CmdBuffer &&);
    CmdBuffer(const CmdBuffer &) = delete;

    CmdBuffer & operator=(CmdBuffer &&) = delete;
    CmdBuffer & operator=(const CmdBuffer &) = delete;

private:
    vk::CommandBuffer _buffer;
    vk::CommandPool   _pool;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DEVICES_CMDBUFFER_HPP