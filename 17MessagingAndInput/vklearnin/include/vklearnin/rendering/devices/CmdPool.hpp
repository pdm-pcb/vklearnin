#ifndef VKLEARNIN_RENDERING_DEVICES_CMDPOOL_HPP
#define VKLEARNIN_RENDERING_DEVICES_CMDPOOL_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdPool final {
public:
    void reset(const vk::CommandPoolResetFlags flags = { }) const;

    void create();
    void destroy();

    inline const auto & native() const { return _pool; }

    CmdPool() = default;
    ~CmdPool() = default;

    CmdPool(CmdPool &&);
    CmdPool(const CmdPool &) = delete;

    CmdPool & operator=(CmdPool &&) = delete;
    CmdPool & operator=(const CmdPool &) = delete;

private:
    vk::CommandPool _pool;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DEVICES_CMDPOOL_HPP