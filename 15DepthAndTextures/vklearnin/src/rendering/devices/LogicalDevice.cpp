#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"

#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/CmdQueue.hpp"
#include "vklearnin/rendering/devices/CmdPool.hpp"

namespace vkl {

CmdQueue   LogicalDevice::_cmd_queue;
CmdPool    LogicalDevice::_cmd_pool;
vk::Device LogicalDevice::_logical_device = nullptr;

// =============================================================================
void LogicalDevice::create() {
    // There's a strong probability that on a given device, these two indices
    // will have the same value. None the less, it's prudent to double-check,
    // and std::set lends a hand here
    const auto family_indices = PhysicalDevice::family_indices();
    if(family_indices.size() != 1) {
        CONSOLE_CRITICAL("A queue which accepts graphics commands must also "
                         "accept present commands.");
    }

    _cmd_queue.populate_info(family_indices.front());

    // Once the queues have their own creation info, LogicalDevice can continue
    vk::DeviceQueueCreateInfo queue_info[] {
        _cmd_queue.create_info()
    };

    std::vector<const char *> layers;
#ifdef VKL_DEBUG
        layers.push_back("VK_LAYER_KHRONOS_validation");
#endif // VKL_DEBUG

    std::vector<const char *> extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::DeviceCreateInfo device_info {
        .queueCreateInfoCount    = static_cast<uint32_t>(std::size(queue_info)),
        .pQueueCreateInfos       = queue_info,
        .enabledLayerCount       = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames     = layers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures        = nullptr,
    };

    auto result = PhysicalDevice::native().createDevice(
        &device_info,
        nullptr,
        &_logical_device
    );

    CONSOLE_TRACE(
        "Created logical device {:#x}",
        reinterpret_cast<uint64_t>(VkDevice(native()))
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create logical device.");
    }

    _cmd_queue.request_queue();
    _cmd_pool.create();

    // This is the final step in providing the dynamic loader with information
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_logical_device);
}

// =============================================================================
void LogicalDevice::destroy() {
    _cmd_pool.destroy();

    CONSOLE_TRACE(
        "Destroying logical device {:#x}",
        reinterpret_cast<uint64_t>(VkDevice(native()))
    );

    _logical_device.destroy();
}

} // namespace vkl