#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/DeviceQueue.hpp"

#include "vklearnin/rendering/PhysicalDevice.hpp"

namespace vkl {

// =============================================================================
// Arguably, this isn't enough to be a function on its own, but for now it seems
// like the logical choice
void DeviceQueue::request_queues(vk::Device const *const logical_device) {
    // We'll be using this
    _logical_device = logical_device;
    for(auto &queue : _cmd_queues) {
        queue = _logical_device->getQueue(_family_index, 0u);
        if(!queue) {
            CONSOLE_CRITICAL("Could not get device queue");
        }
    }
}

// =============================================================================
void DeviceQueue::create_pools() {

    // Once we've established the family to which this particular queue belongs,
    // there's not much else to specify during the creation of its command pool
    vk::CommandPoolCreateInfo pool_info {
        .flags = { },
        .queueFamilyIndex = _queue_create_info.queueFamilyIndex,
    };

    for(auto &pool : _cmd_pools) {
        auto result = _logical_device->createCommandPool(
            &pool_info,
            nullptr,
            &pool
        );

        if(result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Unable to create command pool.");
        }
        else {
            CONSOLE_TRACE(
                "Created command pool {}.",
                reinterpret_cast<uint64_t>(VkCommandPool(pool))
            );
        }
    }
}

// =============================================================================
void DeviceQueue::allocate_cmd_buffers() {
    for(uint32_t image = 0u;
        image < RenderConfig::swapchain_image_count;
        ++image)
    {
        vk::CommandBufferAllocateInfo buffer_info {
            .commandPool = _cmd_pools[image],
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1u,
        };

        auto result = _logical_device->allocateCommandBuffers(
            &buffer_info,
            &_cmd_buffers[image]
        );

        if(result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Unable to allocate command buffer");
        }
    }
}

// =============================================================================
// DeviceQueue's constructor has a little more going on than most, because the
// below information is required for the creation of the logical device which
// owns it
DeviceQueue::DeviceQueue(const uint32_t family_index, const float priority) :
    _family_index   { family_index },
    _queue_priority { priority },
    _logical_device { nullptr }
{
    _queue_create_info = vk::DeviceQueueCreateInfo {
        .queueFamilyIndex = _family_index,
        .queueCount = 1u,
        .pQueuePriorities = &_queue_priority,
    };

    _cmd_queues.resize(RenderConfig::swapchain_image_count);
    _cmd_pools.resize(RenderConfig::swapchain_image_count);
    _cmd_buffers.resize(RenderConfig::swapchain_image_count);
}

DeviceQueue::~DeviceQueue() {
    // Only via the call to std::vector::clear() in the logical device
    // responsible for this queue can we be sure that the buffers and pool
    // will be properly disposed of

    for(uint32_t image = 0;
        image < RenderConfig::swapchain_image_count;
        ++image)
    {
        CONSOLE_TRACE(
            "Destroying command pool {}.",
            reinterpret_cast<uint64_t>(VkCommandPool(_cmd_pools[image]))
        );
        _logical_device->freeCommandBuffers(_cmd_pools[image],
                                            _cmd_buffers[image]);
        _logical_device->destroyCommandPool(_cmd_pools[image]);
    }
}

DeviceQueue::DeviceQueue(DeviceQueue &&other) :
    _queue_create_info { std::move(other._queue_create_info) },
    _queue_priority    { std::move(other._queue_priority)    },
    _cmd_queues        { std::move(other._cmd_queues)        },
    _cmd_pools         { std::move(other._cmd_pools)         },
    _cmd_buffers       { std::move(other._cmd_buffers)       },
    _logical_device    { std::move(other._logical_device)    }
{ }

} // namespace vkl