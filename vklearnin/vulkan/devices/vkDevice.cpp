#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"

namespace vkl {

// =============================================================================
bool vkDevice::create(vkPhysicalDevice const &physical_device) {
    if(_handle) {
        Log::error("Device {} already created.", _handle);
        return false;
    }

    if(!physical_device.native()) {
        Log::error("Cannot create device with invalid physical device.");
        return false;
    }

    _physical_device = &physical_device;

    // We only need one device queue, so only need to specify one priority
    float const queue_priorities[] = { 1.0f };

    // Populate the device queue create struct
    vk::DeviceQueueCreateInfo const queue_create_info[] {{
        .flags = { },
        .queueFamilyIndex = physical_device.graphics_queue_index(),
        .queueCount = static_cast<uint32_t>(std::size(queue_priorities)),
        .pQueuePriorities = queue_priorities,
    }};

    // The logical device concerns itself with the features and extensions
    // enabled on the physical device
    auto const &features = physical_device.features();
    auto const &extensions = physical_device.extensions();

    std::vector<char const *> extension_names;
    extension_names.reserve(extensions.size());

    for(auto const &extension : extensions) {
        extension_names.emplace_back(extension.extensionName);
    }

    // Now populate the device's create struct
    vk::StructureChain<vk::DeviceCreateInfo,
                       vk::PhysicalDeviceFeatures2> device_info = {
        vk::DeviceCreateInfo {
            .flags = { },
            .queueCreateInfoCount =
                static_cast<uint32_t>(std::size(queue_create_info)),
            .pQueueCreateInfos = queue_create_info,
            .enabledExtensionCount =
                static_cast<uint32_t>(extension_names.size()),
            .ppEnabledExtensionNames = extension_names.data(),
            // This is nullptr, since we're using PhysicalDeviceFeatures2
            .pEnabledFeatures = nullptr,
        },
        features,
    };

    // And try to create it
    _handle = physical_device.native().createDevice(device_info.get());
    Log::trace("Created logical device {}.", _handle);

    // Set up the queue abstraction
    _graphics_queue.set(*this, physical_device.graphics_queue_index());

    // This is the final step in providing the dynamic loader with information
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_handle);

    return true;
}

// =============================================================================
bool vkDevice::destroy() {
    if(!_handle) {
        Log::error("Must create device before calling destroy.");
        return false;
    }

    _physical_device = nullptr;

    _graphics_queue.clear();

    Log::trace("Destroying logical device {}", _handle);
    _handle.destroy();
    _handle = nullptr;

    return true;
}

// =============================================================================
void vkDevice::wait_idle() const {
    Log::trace("Waiting for device idle...");
    _handle.waitIdle();
}

} // namespace vkl