#include "vklearnin/common.hpp"
#include "vklearnin/CommandStructures/CommandQueues.hpp"

#include "vklearnin/Instance.hpp"

#include <set>

#if defined(__linux__)
    #include "vklearnin/Platform/X11/X11Window.hpp"
#elif defined(_WIN32)
    #include "vklearnin/Platform/Win32/Win32Window.hpp"
#endif

// =============================================================================
void CommandQueues::init_families() {
    CONSOLE_INFO("");

    std::vector<vk::QueueFamilyProperties> family_props =
        _instance.physical_device().getQueueFamilyProperties();

    CONSOLE_TRACE("Found {} queue families", family_props.size());

    // run through each queue family to establish the first which can both
    // draw and present
    std::vector<vk::Bool32> present_support(family_props.size());

    auto flags = (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer);

    for(uint32_t queue = 0; queue < family_props.size(); ++queue) {
        // grab the surface info for this command queue family
        vkGetPhysicalDeviceSurfaceSupportKHR(
            _instance.physical_device(),
            queue,
            _surface,
            &present_support[queue]
        );

        // if this family can both draw and present, we've got the queue we
        // want
        if(family_props[queue].queueFlags & flags) {
            if(present_support[queue] == VK_TRUE) {
                _graphics_family = queue;
                _present_family = queue;
                break;
            }
        }
    }

    if(is_complete() == false) {
        CONSOLE_ERROR("Could not find a suitable command queue family.");
    }
}

// =============================================================================
void CommandQueues::init_queue_info() {
    CONSOLE_INFO("");

    // The queue indices for the graphics bit and presentation are likely to be
    // the same, at least for now?
    std::set<uint32_t> unique_command_queues = {
        graphics_index(),
        present_index()
    };

    _queue_info_structs.reserve(unique_command_queues.size());
    _queue_priorities.reserve(unique_command_queues.size());

    // I'm suspicious of setting queueCount to 1 for each iteration of this
    // loop, but at present there really is only one queue, so...
    for(const uint32_t family : unique_command_queues) {
        _queue_priorities.emplace_back(1.0f);
        _queue_info_structs.emplace_back(
            vk::DeviceQueueCreateInfo {
                .queueFamilyIndex = family,
                .queueCount = static_cast<uint32_t>(_queue_priorities.size()),
                .pQueuePriorities = _queue_priorities.data(),
            }
        );
    }

    CONSOLE_TRACE(
        "Accumulated {} queue family {} for graphics and present",
        _queue_info_structs.size(),
        _queue_info_structs.size() == 1 ? "index" : "indices"
    );
}

// =============================================================================
void CommandQueues::init_pools() {
    CONSOLE_INFO("");

    // As I've just learned, a command pool is the memory from which any given
    // number of command buffers may be allocated. However, they are not
    // synchronized at all, so each thread must have separate pools.
    vk::CommandPoolCreateInfo pool_info {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = graphics_index()
    };

    auto result = _instance.logical_device().createCommandPool(
        &pool_info,
        nullptr,
        &_command_pool
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Unable to create graphics command pool.");
    }
    else {
        CONSOLE_TRACE("Created graphics command pool");
    }
}

// =============================================================================
void CommandQueues::init_queues() {
    CONSOLE_INFO("");

    // retrieve the graphics command queue
    _instance.logical_device().getQueue(
        graphics_index(),
        0u,
        &_graphics_queue
    );

    // retrieve the presentation command queue
    _instance.logical_device().getQueue(
        present_index(),
        0u,
        &_present_queue
    );
}

// =============================================================================
void CommandQueues::init_buffers() {
    CONSOLE_INFO("");

    vk::CommandBufferAllocateInfo buffer_info {
        .commandPool = _command_pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 
            static_cast<uint32_t>(_command_buffers.size())
    };

    CONSOLE_TRACE(
        "Allocating {} command {}",
        buffer_info.commandBufferCount,
        buffer_info.commandBufferCount == 1 ? "buffer" : "buffers"
    );

    auto result = _instance.logical_device().allocateCommandBuffers(
        &buffer_info,
        _command_buffers.data()
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Unable to allocate command buffer");
    }
}

// =============================================================================
CommandQueues::CommandQueues(const vk::SurfaceKHR &surface,
                             const Instance &instance) :
    _graphics_queue  { nullptr  },
    _present_queue   { nullptr  },
    _command_pool    { nullptr  },
    _command_buffers { nullptr  },
    _surface         { surface  },
    _instance        { instance }
{
    CONSOLE_INFO("");
}

CommandQueues::~CommandQueues() {
    _instance.logical_device().destroy(_command_pool);
}