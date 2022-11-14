#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/LogicalDevice.hpp"

#include "vklearnin/rendering/PhysicalDevice.hpp"

namespace vkl {

// =============================================================================
void LogicalDevice::init() {
    // There's a strong probability that on a given device, these two indices
    // will have the same value. None the less, it's prudent to double-check,
    // and std::set lends a hand here
    const auto family_indices = PhysicalDevice::current().family_indices();
    _device_queues.reserve(family_indices.size());

    // Once the queues have their own creation info, LogicalDevice can continue
    std::vector<vk::DeviceQueueCreateInfo> queue_info;
    queue_info.reserve(family_indices.size());

    for(const auto index : family_indices) {
        _device_queues.emplace_back(index, 1.0f);
        queue_info.emplace_back(_device_queues.back().create_info());
    }

    std::vector<const char *> layers;
#ifdef VKL_DEBUG
        layers.push_back("VK_LAYER_KHRONOS_validation");
#endif // VKL_DEBUG

    std::vector<const char *> extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::DeviceCreateInfo device_info {
        .queueCreateInfoCount = static_cast<uint32_t>(queue_info.size()),
        .pQueueCreateInfos = queue_info.data(),
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = nullptr,
    };

    auto result = PhysicalDevice::current().native().createDevice(
        &device_info,
        nullptr,
        &_logical_device
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create logical device.");
    }

    // This is the final step in providing the dynamic loader with information
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_logical_device);

    _init_device_queues();
}

// =============================================================================
const DeviceQueue & LogicalDevice::graphics_queue() const {
    return _device_queues[PhysicalDevice::current().graphics_queue_index()];
}

// =============================================================================
const DeviceQueue & LogicalDevice::present_queue() const {
    return _device_queues[PhysicalDevice::current().present_queue_index()];
}

// =============================================================================
const DeviceQueue & LogicalDevice::default_queue() const {
    return _device_queues.front();
}

// =============================================================================
void LogicalDevice::_init_device_queues() {
    // For each queue associated with this device, proceed to create the thing,
    // as well as its associated buffers
    for(auto &queue : _device_queues) {
        queue.request_queues(&_logical_device);
        queue.create_pools();
        queue.allocate_cmd_buffers();
    }
}

// =============================================================================
LogicalDevice::LogicalDevice() :
    _logical_device  { nullptr }
{ }

LogicalDevice::~LogicalDevice() {
    // It's important to explicitly clear the vector so the DeviceQueue
    // destructor will be called
    _device_queues.clear();

    // Regarding the above: while a vk::Queue will be destroyed with its parent
    // logical device, any pools or buffers it's created along the way will not
    _logical_device.destroy();
}

} // namespace vkl