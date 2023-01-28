#ifndef VKLEARNIN_SYSTEM_DEVICES_CMDPOOL_HPP
#define VKLEARNIN_SYSTEM_DEVICES_CMDPOOL_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdPool final {
public:
    void reset(const vk::CommandPoolResetFlags flags = { }) const;

    void create(const vk::CommandPoolCreateFlags flags = { });
    void destroy();

    inline auto & native() const { return _pool; }

    CmdPool();
    ~CmdPool() = default;

    CmdPool(CmdPool &&other) noexcept;
    CmdPool(const CmdPool &) = delete;

    CmdPool& operator=(CmdPool &&) = delete;
    CmdPool& operator=(const CmdPool &) = delete;

private:
    vk::CommandPool _pool;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_DEVICES_CMDPOOL_HPP