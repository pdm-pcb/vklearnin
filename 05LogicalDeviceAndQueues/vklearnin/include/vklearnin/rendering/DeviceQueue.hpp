#ifndef VKLEARNIN_RENDERING_DEVICEQUEUE_HPP
#define VKLEARNIN_RENDERING_DEVICEQUEUE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class DeviceQueue final {
public:
    // Memory from which the queues will be allocated
    void create_pool(vk::Device const *const logical_device);
    // Queue into which commands will be placed
    void request_queue();
    // Buffers which shall hold the queued commands
    void allocate_cmd_buffer();

    inline void reset_cmd_pool(const vk::CommandPoolResetFlags flags = { })
    {
        _logical_device->resetCommandPool(_cmd_pool, flags);
    }

    // For those concerned with the internal state of DeviceQueue
    inline const auto & create_info() const { return _queue_create_info; }
    inline const auto & native()      const { return _cmd_queue;         }
    inline const auto & cmd_pool()    const { return _cmd_pool;          }
    inline const auto & cmd_buffer()  const { return _cmd_buffer;        }

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
    vk::CommandPool   _cmd_pool;
    vk::CommandBuffer _cmd_buffer;
    vk::Queue         _cmd_queue;

    const vk::Device *_logical_device;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DEVICEQUEUE_HPP