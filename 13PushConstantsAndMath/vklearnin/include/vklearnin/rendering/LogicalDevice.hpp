#ifndef VKLEARNIN_RENDERING_LOGICALDEVICE_HPP
#define VKLEARNIN_RENDERING_LOGICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/DeviceQueue.hpp"
#include "vklearnin/rendering/PhysicalDevice.hpp"

namespace vkl {

class LogicalDevice final {
public:
    static void init();
    static void shutdown();

    inline static auto & native() { return _logical_device; }
    inline static auto & queues() { return _device_queues; }
    
    inline static const DeviceQueue & graphics_queue() {
        return _device_queues[PhysicalDevice::graphics_queue_index()];
    }
    inline static const DeviceQueue & present_queue() {
        return _device_queues[PhysicalDevice::present_queue_index()];
    }
    inline static const DeviceQueue & default_queue() {
        return _device_queues.front();
    }

    LogicalDevice() = delete;
    ~LogicalDevice() = delete;

    LogicalDevice(LogicalDevice &&other) = delete;
    LogicalDevice(const LogicalDevice &other) = delete;

    LogicalDevice & operator=(LogicalDevice &&other) = delete;
    LogicalDevice & operator=(const LogicalDevice &other) = delete;

private:
    static vk::Device               _logical_device;
    static std::vector<DeviceQueue> _device_queues;

    static void _init_device_queues();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_LOGICALDEVICE_HPP