#ifndef VKLEARNIN_RENDERING_DEVICES_LOGICALDEVICE_HPP
#define VKLEARNIN_RENDERING_DEVICES_LOGICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdQueue;
class CmdPool;

class LogicalDevice final {
public:
    static void create();
    static void destroy();

    inline static const auto & native()    { return _logical_device; }
    inline static const auto & cmd_queue() { return _cmd_queue; }
    inline static const auto & cmd_pool()  { return _cmd_pool; }

    LogicalDevice() = delete;
    ~LogicalDevice() = delete;

    LogicalDevice(LogicalDevice &&other) = delete;
    LogicalDevice(const LogicalDevice &other) = delete;

    LogicalDevice & operator=(LogicalDevice &&other) = delete;
    LogicalDevice & operator=(const LogicalDevice &other) = delete;

private:
    static CmdQueue   _cmd_queue;
    static CmdPool    _cmd_pool;
    static vk::Device _logical_device;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DEVICES_LOGICALDEVICE_HPP