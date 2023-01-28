#ifndef VKLEARNIN_SYSTEM_DEVICES_LOGICALDEVICE_HPP
#define VKLEARNIN_SYSTEM_DEVICES_LOGICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdQueue;

class LogicalDevice final {
public:
    static void create();
    static void destroy();

    inline static const auto & native()    { return _logical_device; }
    inline static const auto & cmd_queue() { return _cmd_queue; }

    LogicalDevice() = delete;

private:
    static CmdQueue _cmd_queue;
    static vk::Device _logical_device;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_DEVICES_LOGICALDEVICE_HPP