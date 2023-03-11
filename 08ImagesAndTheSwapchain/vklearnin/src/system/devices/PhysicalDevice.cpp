#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/devices/PhysicalDevice.hpp"

#include "vklearnin/system/window/TargetWindow.hpp"

namespace vkl {

PhysicalDevice::DeviceList PhysicalDevice::_available_devices;
vk::PhysicalDevice           PhysicalDevice::_physical_device { };

uint32_t PhysicalDevice::_queue_index = std::numeric_limits<uint32_t>::max();

// =============================================================================
void PhysicalDevice::query_devices() {
    // Query and populate the list of physical devices
    auto [enumdev_result, devices] =
        GraphicsAPI::native().enumeratePhysicalDevices();
    if(enumdev_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to enumerate physical devices: '{}'",
            to_string(enumdev_result)
        );
        return;
    }
    
    if(devices.empty()) {
        CONSOLE_CRITICAL("No physical devices found.");
        return;
    }

    CONSOLE_TRACE("Found {} {}", devices.size(),
                  (devices.size() == 1 ? "device" : "devices"));

    for(const auto &device : devices) {
        const auto &props = device.getProperties();

        // We'll want to know what extensions the devices support
        auto [enumext_result, extensions] =
            device.enumerateDeviceExtensionProperties();
        
        if(enumext_result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL(
                "Failed to enumerate device extensions: '{}'",
                to_string(enumext_result)
        	);
        }
        CONSOLE_TRACE("Found {} physical device extensions", extensions.size());

        // Swapchain support is definitely required
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

        // Here's some extra property gathering so we can establish which of
        // the available devices has the most VRAM
        const auto &memory = device.getMemoryProperties();

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
            "\tDevice Type:    {}\n"
            "\tVRAM:           {} MB\n"
            "\tDriver Version: {}\n"
            "\tVulkan Version: {}\n",
            properties.name,
            to_string(properties.type),
            properties.vram_bytes / 1000 / 1000,
            properties.driver_version,
            properties.vkapi_version
        );
    }

    // Reverse sort the devices based on amount of VRAM, favoring dGPUs
    std::sort(_available_devices.begin(), _available_devices.end(),
        [&](const DeviceProps &dev_a, const DeviceProps &dev_b) {
            if(dev_a.type == vk::PhysicalDeviceType::eDiscreteGpu &&
               dev_b.type != vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return true;
            }
            
            if((dev_a.type == vk::PhysicalDeviceType::eDiscreteGpu &&
                dev_b.type == vk::PhysicalDeviceType::eDiscreteGpu) ||
               (dev_a.type == vk::PhysicalDeviceType::eIntegratedGpu &&
                dev_b.type == vk::PhysicalDeviceType::eIntegratedGpu))
            {
                return dev_a.vram_bytes > dev_b.vram_bytes;
            }

            return false;
        }
    );
}

// =============================================================================
// In order to render, we need to ensure the graphics card support receiving
// two types of commands: graphics and present. The latter requires an existing
// surface to query, so here we go.
void PhysicalDevice::select_device() {
    const auto &surface = TargetWindow::surface();

    // Set up our hopefully to-be-rectified failure conditions
    std::vector<std::pair<bool, uint32_t>> graphics_support;
    std::vector<std::pair<bool, uint32_t>> present_support;
    graphics_support.resize(_available_devices.size());
    present_support.resize(_available_devices.size());
    std::fill(graphics_support.begin(), graphics_support.end(),
              std::make_pair(false, std::numeric_limits<uint32_t>::max()));
    std::fill(present_support.begin(), present_support.end(),
              std::make_pair(false, std::numeric_limits<uint32_t>::max()));

    // Loop through all available hardware
    for(uint32_t device_idx = 0u;
        device_idx < _available_devices.size();
        ++device_idx)
    {
        // Ask Vulkan for some details
        const auto &gpu = _available_devices[device_idx].device;
        const auto props = gpu.getQueueFamilyProperties();

        CONSOLE_TRACE("Found {} queue families for {}",
                      props.size(), _available_devices[device_idx].name);

        for(uint32_t family = 0u; family < props.size(); ++family) {
            _print_family_flags(family, props[family].queueFlags);

            // If the current queue family has the graphics bit set, keep track
            if(!graphics_support[device_idx].first &&
               props[family].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                graphics_support[device_idx] = { true, family };
            }

            // If we don't have present support yet, check this queue family
            // against the surface we created earlier. getSurfaceSupportKHR()
            // actually just checks whether a given queue family can present
            // on a given surface. Poorly named function, but oh well.
            if(!present_support[device_idx].first) {
                auto result = gpu.getSurfaceSupportKHR(family, surface);

                // Oblige Vulkan-Hpp and check the return value
                if(result.result != vk::Result::eSuccess) {
                    CONSOLE_CRITICAL(
                        "Failed to query surface support: '{}'",
                        to_string(result.result)
                    );
                }
                else {
                    present_support[device_idx] = { true, family };
                }
            }
        }
    }

    uint32_t gfx_queue_index = 0u;
    uint32_t present_queue_index = 0u;

    for(uint32_t device_index = 1u;
        device_index < _available_devices.size();
        ++device_index)
    {
        const bool gfx = graphics_support[device_index].first;
        const uint32_t gfx_fam = graphics_support[device_index].second;

        const bool present = present_support[device_index].first;
        const uint32_t present_fam = present_support[device_index].second;

        vk::PhysicalDeviceFeatures features { };
        _available_devices[device_index].device.getFeatures(&features);

        if(gfx && present && gfx_fam != std::numeric_limits<uint32_t>::max() &&
           present_fam != std::numeric_limits<uint32_t>::max() &&
           features.samplerAnisotropy != 0)
        {
            const auto &dev_store = _available_devices[device_index];
            _physical_device      = dev_store.device;
            gfx_queue_index       = graphics_support[device_index].second;
            present_queue_index   = present_support[device_index].second;

            CONSOLE_TRACE(
                "Selected {}, queue {} for graphics and {} for present",
                dev_store.name,
                gfx_queue_index,
                present_queue_index
            );

            // Just choose the first satisfactory device, as they're already
            // sorted by VRAM
            break;
        }
    }

    // This would be most unfortunate
    if(!_physical_device) {
        CONSOLE_CRITICAL("Could not find a device with support for a graphics "
                         "and present command queues.");
    }
    
    if(gfx_queue_index != present_queue_index) {
        CONSOLE_CRITICAL("Device must support drawing and presenting in a "
                         "single queue.");
    }

    _queue_index = gfx_queue_index;
}

// =============================================================================
void PhysicalDevice::_store_physical_device(
    const vk::PhysicalDevice &device,
    const vk::PhysicalDeviceProperties &properties,
    const vk::PhysicalDeviceMemoryProperties &memory,
    const vk::PhysicalDeviceDriverProperties &drivers)
{
    DeviceProps store { };
    store.device = device;
    store.type = properties.deviceType;
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
void
PhysicalDevice::_print_family_flags([[maybe_unused]] const uint32_t family,
                                    [[maybe_unused]] const vk::QueueFlags flags)
{
#ifdef VKL_DEBUG
    std::stringstream flags_stream;
    flags_stream << family << ": ";

    if(flags & vk::QueueFlagBits::eGraphics) {
        flags_stream << "Graphics       ";
    }
    if(flags & vk::QueueFlagBits::eCompute) {
        flags_stream << "Compute        ";
    }
    if(flags & vk::QueueFlagBits::eTransfer) {
        flags_stream << "Transfer       ";
    }
    if(flags & vk::QueueFlagBits::eSparseBinding) {
        flags_stream << "Sparse Binding ";
    }
    if(flags & vk::QueueFlagBits::eProtected) {
        flags_stream << "Protected      ";
    }

    CONSOLE_TRACE("    {}", flags_stream.str());
#endif // VKL_DEBUG
}

} // namespace vkl