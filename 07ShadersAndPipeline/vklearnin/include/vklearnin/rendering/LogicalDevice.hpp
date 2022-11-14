#ifndef VKLEARNIN_RENDERING_LOGICALDEVICE_HPP
#define VKLEARNIN_RENDERING_LOGICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class PhysicalDevice;
class DeviceQueue;

class LogicalDevice final {
public:
    void init();

    inline const vk::Device & native() const { return _logical_device; }
    inline auto & queues() { return _device_queues; }
    
    DeviceQueue & graphics_queue();
    DeviceQueue & present_queue();

    explicit LogicalDevice(const PhysicalDevice &physical_device);
    ~LogicalDevice();

    LogicalDevice() = delete;

    LogicalDevice(LogicalDevice &&other) = delete;
    LogicalDevice(const LogicalDevice &other) = delete;

    LogicalDevice & operator=(LogicalDevice &&other) = delete;
    LogicalDevice & operator=(const LogicalDevice &other) = delete;

private:
    vk::Device               _logical_device;
    std::vector<DeviceQueue> _device_queues;
    const PhysicalDevice    &_physical_device;

    void _init_device_queues();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_LOGICALDEVICE_HPP