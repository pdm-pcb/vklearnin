#ifndef VKLEARNIN_SYSTEM_DEVICES_LOGICALDEVICE_HPP
#define VKLEARNIN_SYSTEM_DEVICES_LOGICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/system/devices/CmdQueue.hpp"
#include "vklearnin/system/devices/CmdPool.hpp"

namespace vkl {

class LogicalDevice final {
public:
    static void create();
    static void destroy();

    inline static auto const& native()    { return _logical_device; }
    inline static auto const& cmd_queue() { return _cmd_queue;      }
    inline static auto& transient_pool()  { return _transient_pool; }

    LogicalDevice() = delete;

private:
    static CmdQueue   _cmd_queue;
    static CmdPool    _transient_pool;
    static vk::Device _logical_device;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_DEVICES_LOGICALDEVICE_HPP