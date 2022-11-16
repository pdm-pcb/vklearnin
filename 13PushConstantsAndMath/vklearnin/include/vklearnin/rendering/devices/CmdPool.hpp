#ifndef VKLEARNIN_RENDERING_DEVICES_CMDPOOL_HPP
#define VKLEARNIN_RENDERING_DEVICES_CMDPOOL_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdPool {
public:
    void reset(const vk::CommandPoolResetFlags flags = { }) const;

    inline const auto & native() const { return _pool; }

    CmdPool() = default;
    ~CmdPool() = default;

    CmdPool(CmdPool &&) = delete;
    CmdPool(const CmdPool &) = delete;

    CmdPool & operator=(CmdPool &&) = delete;
    CmdPool & operator=(const CmdPool &) = delete;

private:
    vk::CommandPool _pool;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DEVICES_CMDPOOL_HPP