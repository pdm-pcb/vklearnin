#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/PhysicalDevice.hpp"

#include "vklearnin/rendering/GraphicsInstance.hpp"

namespace vkl {

// =============================================================================
void PhysicalDevice::init(const vk::Instance &graphics_instance) {
    // Query and populate the list of physical devices
    auto [enumdev_result, devices] =
        graphics_instance.enumeratePhysicalDevices();
    if(enumdev_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to enumerate physical devices.");
    }
    CONSOLE_TRACE("Found {} {}", devices.size(),
                  (devices.size() == 1 ? "device" : "devices"));

    for(auto device : devices) {
        auto props = device.getProperties();

        // We're not interested in software rendering
        if(props.deviceType != vk::PhysicalDeviceType::eDiscreteGpu &&
           props.deviceType != vk::PhysicalDeviceType::eIntegratedGpu)
        {
            continue;
        }

        // We'll want to know what extensions the devices support
        auto [enumext_result, extensions] =
            device.enumerateDeviceExtensionProperties();
        
        if(enumext_result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Failed to enumerate device extensions.");
        }
        CONSOLE_TRACE("Found {} physical device extensions", extensions.size());

        bool swapchain_support = false;
        for(const auto &extension : extensions) {
            if(strcmp(extension.extensionName,
                      VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
            {
                swapchain_support = true;
                break;
            }
        }

        if(!swapchain_support) {
            CONSOLE_WARN(
                "{} does not suppoprt the swapchain extension.",
                props.deviceName
            );
            continue;
        }

        CONSOLE_TRACE("Found swapchain support");

        // Here's some extra property gathering so we can establish which of
        // the available devices has the most VRAM
        auto memory = device.getMemoryProperties();

        // Yet more property gathering - this time so we can get a driver
        // version string that matches what the hardware vendor publishes
        vk::PhysicalDeviceDriverProperties driver_props { };
        vk::PhysicalDeviceProperties2KHR physical_props2 {
            .pNext = &driver_props
        };
        
        // Run through what extensions we have until we find the driver info
        for(const auto &extension : extensions) {
            if(strcmp(extension.extensionName,
                      VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME) == 0)
            {
                device.getProperties2(&physical_props2);
                break;
            }
        }

        // Hold onto the info we've gathered
        _store_physical_device(device, props, memory, driver_props);
        const auto &properties = _available_devices.back();

        CONSOLE_TRACE(
            "\n"
            "\tDevice Name:    {}\n"
            "\tVRAM:           {}MB\n"
            "\tDriver Version: {}\n"
            "\tVulkan Version: {}\n",
            properties.name,
            properties.vram_bytes / 1000 / 1000,
            properties.driver_version,
            properties.vkapi_version
        );
    }
}

// =============================================================================
// In order to render, we need to ensure the graphics card support receiving
// two types of commands: graphics and present. The latter requires an existing
// surface to query, so here we go.
void PhysicalDevice::select_device(const vk::SurfaceKHR &surface) {
    // Reverse sort the devices based on amount of VRAM
    std::sort(_available_devices.begin(), _available_devices.end(),
        [&](const DeviceProperties &a, const DeviceProperties &b) {
            return a.vram_bytes > b.vram_bytes;
        }
    );

    // Set up our hopefully to-be-rectified failure conditions
    bool graphics_support = false;
    bool present_support  = false;
    uint32_t device_index = std::numeric_limits<uint32_t>::max();

    // Loop through all available hardware
    for(uint32_t device = 0u; device < _available_devices.size(); ++device) {
        // Ask Vulkan for some details
        const auto &vk_physical_dev = _available_devices[device].device;
        const auto props = vk_physical_dev.getQueueFamilyProperties();

        CONSOLE_TRACE("Found {} queue families for device {}\n",
                      props.size(), _available_devices[device].name);

        for(uint32_t family = 0u; family < props.size(); ++family) {
            // If the current queue family has the graphics bit set, keep track
            if(!graphics_support &&
               props[family].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                graphics_support = true;
                _graphics_queue_index = family;
                device_index = device;
            }

            // If we don't have present support yet, check this queue family
            // against the surface we created earlier
            if(!present_support) {
                auto result =
                    vk_physical_dev.getSurfaceSupportKHR(family, surface);

                // Oblige Vulkan-Hpp and check its return values
                if(result.result != vk::Result::eSuccess) {
                    CONSOLE_CRITICAL("Failed to query surface support.");
                }
                if(result.value) {
                    present_support = true;
                    _present_queue_index = family;
                    device_index = device;

                    // We've got what we need, and the cards are already sorted
                    // by VRAM
                    if(graphics_support) {
                        break;
                    }
                }
            }
        }

        // Break now, since the cards are already sorted by VRAM
        if(graphics_support && present_support) {
            CONSOLE_TRACE(
                "Chose queue {} for graphics and {} for presentation on {}\n",
                _graphics_queue_index,
                _present_queue_index,
                _available_devices[device].name
            );
            break;
        }

        // This device was of no use to us, so reset and continue
        graphics_support = false;
        present_support  = false;
        device_index = std::numeric_limits<uint32_t>::max();
    }

    // This would be most unfortunate
    if(graphics_support == false && present_support == false) {
        CONSOLE_CRITICAL("Could not find a device with support for a graphics "
                         "command queue.");
    }

    // Pluck the winner from the crowd and let's go home
    _physical_device = _available_devices[device_index].device;
    CONSOLE_TRACE("Selected {}", _available_devices[device_index].name);
}

// =============================================================================
void PhysicalDevice::_store_physical_device(
    const vk::PhysicalDevice &device,
    const vk::PhysicalDeviceProperties &properties,
    const vk::PhysicalDeviceMemoryProperties &memory,
    const vk::PhysicalDeviceDriverProperties &drivers)
{
    DeviceProperties store { };
    store.device = device;
    store.name = std::string(properties.deviceName.data());

    size_t vram_bytes = 0;
    for(uint32_t index = 0u; index < memory.memoryHeapCount; ++index) {
        auto flags = memory.memoryHeaps[index].flags;

        if((flags & vk::MemoryHeapFlagBits::eDeviceLocal) == flags) {
            vram_bytes = memory.memoryHeaps[index].size;
            break;
        }
    }
    store.vram_bytes = vram_bytes;

    store.driver_version = std::string(drivers.driverInfo.data());

    store.vkapi_version = fmt::format(
        "{}.{}.{}",
        VK_API_VERSION_MAJOR(properties.apiVersion),
        VK_API_VERSION_MINOR(properties.apiVersion),
        VK_API_VERSION_PATCH(properties.apiVersion)
    );

    _available_devices.push_back(store);
}

// =============================================================================
PhysicalDevice::PhysicalDevice() :
    _physical_device { nullptr },
    _graphics_queue_index { std::numeric_limits<uint32_t>::max() },
    _present_queue_index  { std::numeric_limits<uint32_t>::max() }
{ }

} // namespace vkl