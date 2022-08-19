#include "common.hpp"
#include "CommandQueues.hpp"

#include "Instance.hpp"

#include <set>

#if defined(__linux__)
    #include "X11Window.hpp"
#elif defined(_WIN32)
    #include "Win32Window.hpp"
#endif

// =============================================================================
void CommandQueues::init_families(const Instance &instance) {
    CONSOLE_INFO("");

    // query and populate the list of command queue families
    uint32_t queue_family_count = 0u;
    ::vkGetPhysicalDeviceQueueFamilyProperties(
        _physical_device,
        &queue_family_count,
        nullptr
    );

    assert(queue_family_count > 0);
    CONSOLE_TRACE("Found {} queue families", queue_family_count);

    std::vector<::VkQueueFamilyProperties> family_props(queue_family_count);
    ::vkGetPhysicalDeviceQueueFamilyProperties(
        _physical_device,
        &queue_family_count,
        family_props.data()
    );

    // run through each queue family to establish the first which can both
    // draw and present
    std::vector<::VkBool32> present_support(queue_family_count);
    for(uint32_t queue = 0; queue < queue_family_count; ++queue) {
        // grab the surface info for this command queue family
        instance._GetPhysicalDeviceSurfaceSupportKHR(
            _physical_device,
            queue,
            _surface,
            &present_support[queue]
        );

        // if this family can both draw and present, we've got the queue we
        // want
        if((family_props[queue].queueFlags & ::VK_QUEUE_GRAPHICS_BIT) != 0) {
            if(present_support[queue] == VK_TRUE) {
                _graphics_family = queue;
                _present_family = queue;
                break;
            }
        }
    }

    if(is_complete() == false) {
        CONSOLE_CRITICAL("Could not find a suitable command queue family.");
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

    // it seems likely there'll be more than one of these at some point in the
    // not-too-distant future.
    _command_pools.emplace_back(::VkCommandPool { });

    ::VkCommandPoolCreateInfo pool_info { };
    pool_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = ::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = graphics_index();

    ::VkResult result = ::vkCreateCommandPool(
        _device,
        &pool_info,
        nullptr,
        &_command_pools.back()
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Unable to create graphics command pool.");
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
        _device,
        graphics_index(),
        0u,
        &_graphics_queue
    );

    if(_graphics_queue == nullptr) {
        CONSOLE_CRITICAL("Could not get logical device's graphics queue.");
    }

    // retrieve the presentation command queue
    ::vkGetDeviceQueue(
        _device,
        present_index(),
        0u,
        &_present_queue
    );

    if(_present_queue == nullptr) {
        CONSOLE_CRITICAL("Could not get logical device's presentation queue.");
    }
}

// =============================================================================
void CommandQueues::init_buffers() {
    CONSOLE_INFO("");

    ::VkCommandBufferAllocateInfo buffer_info { };
    buffer_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_info.commandPool = _command_pools[0];
    buffer_info.level = ::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    buffer_info.commandBufferCount =
        static_cast<uint32_t>(std::size(_command_pools));

    CONSOLE_TRACE(
        "Allocating {} command {}",
        buffer_info.commandBufferCount,
        buffer_info.commandBufferCount == 1 ? "buffer" : "buffers"
    );

    ::VkResult result = ::vkAllocateCommandBuffers(
        _device,
        &buffer_info,
        &_command_buffer
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Unable to allocate command buffer");
    }
}

// =============================================================================
CommandQueues::CommandQueues(const ::VkPhysicalDevice &physical_device,
                             const ::VkDevice         &device,
                             const ::VkSurfaceKHR     &surface) :
    _graphics_queue  { nullptr  },
    _present_queue   { nullptr  },
    _command_buffer  { nullptr  },
    _physical_device { physical_device },
    _device          { device  },
    _surface         { surface }
{
    CONSOLE_INFO("");
}

CommandQueues::~CommandQueues() {
    for(auto pool : _command_pools) {
        ::vkDestroyCommandPool(_device, pool, nullptr);
    }
}