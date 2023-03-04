#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

#include "vklearnin/system/devices/PhysicalDevice.hpp"

namespace vkl {

CmdQueue   LogicalDevice::_cmd_queue      { };
CmdPool    LogicalDevice::_transient_pool { };
vk::Device LogicalDevice::_logical_device { };

// =============================================================================
void LogicalDevice::create() {
    // The first step in device creation is to tell the queue what index it
    // will be using.
    _cmd_queue.fill_create_info(PhysicalDevice::queue_index());

    // Next, ask the queue for the populated structure
    vk::DeviceQueueCreateInfo queue_info[] {
        _cmd_queue.create_info()
    };

    // Provided this is a debug build, we'll want to include the validation
    // layer to afford ourselves the largest possible safety net during
    // development
    std::vector<const char *> layers;
#ifdef VKL_DEBUG
        layers.push_back("VK_LAYER_KHRONOS_validation");
#endif // VKL_DEBUG

    // As this is a graphics application, enable the swapchain extension for
    // whatever surface we're using
    std::vector<const char *> extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // That should be everything we need
    const vk::DeviceCreateInfo device_info {
        .queueCreateInfoCount    = static_cast<uint32_t>(std::size(queue_info)),
        .pQueueCreateInfos       = queue_info,
        .enabledLayerCount       = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames     = layers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures        = &(PhysicalDevice::enabled_features()),
    };

    // Attempt creation
    auto result = PhysicalDevice::native().createDevice(
        &device_info,
        nullptr,
        &_logical_device
    );

    // Make sure it worked
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to create logical device: '{}'",
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE(
            "Created logical device {:#x}",
            reinterpret_cast<uint64_t>(VkDevice(native()))
        );
    }

    // Once the logical device is established, the queue can likewise come
    // online
    _cmd_queue.request_queue();

    // This is the final step in providing the dynamic loader with information
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_logical_device);

    _transient_pool.create(vk::CommandPoolCreateFlagBits::eTransient);
}

// =============================================================================
void LogicalDevice::destroy() {
    CONSOLE_TRACE(
        "Destroying logical device {:#x}",
        reinterpret_cast<uint64_t>(VkDevice(native()))
    );

    _transient_pool.destroy();
    _logical_device.destroy();
}

} // namespace vkl