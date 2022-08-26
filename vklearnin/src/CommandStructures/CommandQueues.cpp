#include "vklearnin/common.hpp"
#include "vklearnin/CommandStructures/CommandQueues.hpp"

#include "vklearnin/Instance.hpp"

#include <set>

#if defined(__linux__)
    #include "vklearnin/Platform/X11/X11Window.hpp"
#elif defined(_WIN32)
    #include "vklearnin/Win32Window.hpp"
#endif

// =============================================================================
void CommandQueues::init_families() {
    CONSOLE_INFO("");

    // query and populate the list of command queue families
    uint32_t queue_family_count = 0u;
    ::vkGetPhysicalDeviceQueueFamilyProperties(
        _instance.physical_device(),
        &queue_family_count,
        nullptr
    );

    assert(queue_family_count > 0);
    CONSOLE_TRACE("Found {} queue families", queue_family_count);

    std::vector<::VkQueueFamilyProperties> family_props(queue_family_count);
    ::vkGetPhysicalDeviceQueueFamilyProperties(
        _instance.physical_device(),
        &queue_family_count,
        family_props.data()
    );

    // run through each queue family to establish the first which can both
    // draw and present
    std::vector<::VkBool32> present_support(queue_family_count);
    for(uint32_t queue = 0; queue < queue_family_count; ++queue) {
        // grab the surface info for this command queue family
        _instance._GetPhysicalDeviceSurfaceSupportKHR(
            _instance.physical_device(),
            queue,
            _surface,
            &present_support[queue]
        );

        // if this family can both draw and present, we've got the queue we
        // want
        if((family_props[queue].queueFlags &
            (::VK_QUEUE_GRAPHICS_BIT | ::VK_QUEUE_TRANSFER_BIT)) != 0)
        {
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

    // If there are actually multiple queue indices, they'll need to be
    // prioritized
    float queue_priorities[] = { 1.0f };

    // I'm suspicious of setting queueCount to 1 for each iteration of this
    // loop, but at present there really is only one queue, so...
    for(const uint32_t family : unique_command_queues) {
        ::VkDeviceQueueCreateInfo queue_info { };
        queue_info.sType = ::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = family;
        queue_info.queueCount = 1u;
        queue_info.pQueuePriorities = queue_priorities;
        _queue_info_structs.emplace_back(queue_info);
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
    ::VkCommandPoolCreateInfo pool_info { };
    pool_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = ::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = graphics_index();

    ::VkResult result = ::vkCreateCommandPool(
        _instance.logical_device(),
        &pool_info,
        nullptr,
        &_command_pool
    );

    if(result != ::VK_SUCCESS) {
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
    ::vkGetDeviceQueue(
        _instance.logical_device(),
        graphics_index(),
        0u,
        &_graphics_queue
    );

    if(_graphics_queue == nullptr) {
        CONSOLE_ERROR("Could not get logical device's graphics queue.");
    }

    // retrieve the presentation command queue
    ::vkGetDeviceQueue(
        _instance.logical_device(),
        present_index(),
        0u,
        &_present_queue
    );

    if(_present_queue == nullptr) {
        CONSOLE_ERROR("Could not get logical device's presentation queue.");
    }
}

// =============================================================================
void CommandQueues::init_buffers() {
    CONSOLE_INFO("");

    ::VkCommandBufferAllocateInfo buffer_info { };
    buffer_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_info.commandPool = _command_pool;
    buffer_info.level = ::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    buffer_info.commandBufferCount =
        static_cast<uint32_t>(std::size(_command_buffers));

    CONSOLE_TRACE(
        "Allocating {} command {}",
        buffer_info.commandBufferCount,
        buffer_info.commandBufferCount == 1 ? "buffer" : "buffers"
    );

    ::VkResult result = ::vkAllocateCommandBuffers(
        _instance.logical_device(),
        &buffer_info,
        _command_buffers.data()
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Unable to allocate command buffer");
    }
}

// =============================================================================
CommandQueues::CommandQueues(const ::VkSurfaceKHR &surface,
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
    if(_command_pool != nullptr) {
        ::vkDestroyCommandPool(
            _instance.logical_device(),
            _command_pool,
            nullptr
        );
    }
}