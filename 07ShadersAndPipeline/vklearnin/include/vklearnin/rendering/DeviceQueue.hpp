#ifndef VKLEARNIN_RENDERING_DEVICEQUEUE_HPP
#define VKLEARNIN_RENDERING_DEVICEQUEUE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class DeviceQueue final {
public:
    // Memory from which the queues will be allocated
    void create_pools(vk::Device const *const logical_device);
    // Queue into which commands will be placed
    void request_queues();
    // Buffers which shall hold the queued commands
    void allocate_cmd_buffers();

    inline void reset_cmd_pool(const uint32_t index,
                               const vk::CommandPoolResetFlags flags = { })
    {
        _logical_device->resetCommandPool(_cmd_pools[index], flags);
    }

    // For those concerned with the internal state of DeviceQueue
    inline const auto & create_info() const { return _queue_create_info; }
    inline const auto & native(const uint32_t index)     const {
        return _cmd_queues[index];
    }
    inline const auto & cmd_pool(const uint32_t index)   const {
        return _cmd_pools[index];
    }
    inline const auto & cmd_buffer(const uint32_t index) const {
        return _cmd_buffers[index];
    }

    // The bare minimum required for a DeviceQueue to know itself
    DeviceQueue(const uint32_t family_index, const float priority);
    ~DeviceQueue();

    DeviceQueue() = delete;

    DeviceQueue(DeviceQueue &&other); // Permitting in-place construction
    DeviceQueue(const DeviceQueue &other) = delete;

    DeviceQueue & operator=(DeviceQueue &&other) = delete;
    DeviceQueue & operator=(const DeviceQueue &other) = delete;

private:
    // Creation-related information for the command queues
    uint32_t _family_index;
    float    _queue_priority;
    vk::DeviceQueueCreateInfo _queue_create_info;

    // Pools from which to allocate the queues, and buffers for the stuffing
    // of commands
    std::vector<vk::CommandPool>   _cmd_pools;
    std::vector<vk::CommandBuffer> _cmd_buffers;
    std::vector<vk::Queue>         _cmd_queues;

    const vk::Device *_logical_device;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DEVICEQUEUE_HPP