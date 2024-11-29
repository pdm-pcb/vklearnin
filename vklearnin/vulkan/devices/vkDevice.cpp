#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"

namespace vkl {

// =============================================================================
bool vkDevice::create(vkPhysicalDevice const &physical_device) {
    if(_handle) {
        Log::error("Device already created.");
        return false;
    }

    if(!physical_device.native()) {
        Log::error("Cannot create device with invalid physical device.");
        return false;
    }

    // We only need one device queue, so only need to specify one priority
    float const queue_priorities[] = { 1.0f };

    // Populate the device queue create struct
    vk::DeviceQueueCreateInfo const queue_create_info[] {{
        .pNext = nullptr,
        .flags = { },
        .queueFamilyIndex = physical_device.cmd_queue_index(),
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
    vk::DeviceCreateInfo const device_create_info {
        .pNext = &features,
        .flags = { },
        .queueCreateInfoCount =
            static_cast<uint32_t>(std::size(queue_create_info)),
        .pQueueCreateInfos = queue_create_info,
        .enabledExtensionCount =
            static_cast<uint32_t>(extension_names.size()),
        .ppEnabledExtensionNames = extension_names.data(),
        .pEnabledFeatures = nullptr,
    };

    // And try to create it
    auto const result = physical_device.native().createDevice(
        &device_create_info, // Create info
        nullptr,             // Allocator
        &_handle             // Destination handle
    );

    // Check that we've got good results to work with
    if(result != vk::Result::eSuccess) {
        Log::critical(
            "Unable to create logical device: '{}'",
            vk::to_string(result)
        );
        return false;
    }

    Log::trace("Created logical device {}", _handle);

    // Set up the queue abstraction
    _cmd_queue.set(*this, physical_device.cmd_queue_index());

    // This is the final step in providing the dynamic loader with information
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_handle);

    _transient_pool.create(
        *this,
        _cmd_queue.family_index(),
        vk::CommandPoolCreateFlagBits::eTransient
    );

    return true;
}

// =============================================================================
bool vkDevice::destroy() {
    if(!_handle) {
        Log::error("Must create device before calling destroy.");
        return false;
    }

    _transient_pool.destroy();

    _cmd_queue.clear();

    Log::trace("Destroying logical device {}", _handle);
    _handle.destroy();
    _handle = nullptr;

    return true;
}

// =============================================================================
bool vkDevice::wait_idle() const {
    Log::trace("Waiting for device idle...");
    auto const result = _handle.waitIdle();

    if(result != vk::Result::eSuccess) {
        Log::error(
            "Failed to wait for device {} idle: '{}'",
            _handle,
            vk::to_string(result)
        );
        return false;
    }

    return true;
}

} // namespace vkl