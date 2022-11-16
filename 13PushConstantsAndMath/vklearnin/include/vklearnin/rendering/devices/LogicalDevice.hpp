#ifndef VKLEARNIN_RENDERING_DEVICES_LOGICALDEVICE_HPP
#define VKLEARNIN_RENDERING_DEVICES_LOGICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/DeviceQueue.hpp"

namespace vkl {

class CmdQueue;
class CmdPool;
class CmdBuffer;

class LogicalDevice final {
public:
    static void create();
    static void destroy();

    inline static auto & native() { return _logical_device; }
    inline static auto & queues() { return _device_queues; }
    
    inline static const DeviceQueue & graphics_queue() {
        return _device_queues[PhysicalDevice::graphics_queue_index()];
    }
    inline static const DeviceQueue & present_queue() {
        return _device_queues[PhysicalDevice::present_queue_index()];
    }

    LogicalDevice() = delete;
    ~LogicalDevice() = delete;

    LogicalDevice(LogicalDevice &&other) = delete;
    LogicalDevice(const LogicalDevice &other) = delete;

    LogicalDevice & operator=(LogicalDevice &&other) = delete;
    LogicalDevice & operator=(const LogicalDevice &other) = delete;

private:
    static vk::Device _logical_device;

    static std::vector<CmdQueue>  _cmd_queues;
    static std::vector<CmdPool>   _cmd_pools;
    static std::vector<CmdBuffer> _cmd_buffers;

    static std::vector<DeviceQueue> _device_queues;

    static void _init_device_queues();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DEVICES_LOGICALDEVICE_HPP