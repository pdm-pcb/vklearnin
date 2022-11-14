#ifndef VKLEARNIN_RENDERING_LOGICALDEVICE_HPP
#define VKLEARNIN_RENDERING_LOGICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/DeviceQueue.hpp"

namespace vkl {

class LogicalDevice final {
public:
    void init();

    inline const vk::Device & native() const { return _logical_device; }
    inline auto & queues() { return _device_queues; }
    
    const DeviceQueue & graphics_queue() const;
    const DeviceQueue & present_queue() const;
    const DeviceQueue & default_queue() const;

    LogicalDevice();
    ~LogicalDevice();

    LogicalDevice(LogicalDevice &&other) = delete;
    LogicalDevice(const LogicalDevice &other) = delete;

    LogicalDevice & operator=(LogicalDevice &&other) = delete;
    LogicalDevice & operator=(const LogicalDevice &other) = delete;

private:
    vk::Device               _logical_device;
    std::vector<DeviceQueue> _device_queues;

    void _init_device_queues();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_LOGICALDEVICE_HPP