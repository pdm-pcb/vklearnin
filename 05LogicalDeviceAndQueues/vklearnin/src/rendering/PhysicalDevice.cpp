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
    std::vector<std::pair<bool, uint32_t>> graphics_support;
    std::vector<std::pair<bool, uint32_t>> present_support;
    graphics_support.resize(_available_devices.size());
    present_support.resize(_available_devices.size());
    std::fill(graphics_support.begin(), graphics_support.end(),
              std::make_pair(false, std::numeric_limits<uint32_t>::max()));
    std::fill(present_support.begin(), present_support.end(),
              std::make_pair(false, std::numeric_limits<uint32_t>::max()));

    // Loop through all available hardware
    for(uint32_t device = 0u; device < _available_devices.size(); ++device) {
        // Ask Vulkan for some details
        const auto &vk_physical_dev = _available_devices[device].device;
        const auto props = vk_physical_dev.getQueueFamilyProperties();

        CONSOLE_TRACE("Found {} queue families for {}",
                      props.size(), _available_devices[device].name);

        for(uint32_t family = 0u; family < props.size(); ++family) {
#ifdef VKL_DEBUG
            _print_family_flags(family, props[family].queueFlags);
#endif // VKL_DEBUG

            // If the current queue family has the graphics bit set, keep track
            if(!graphics_support[device].first &&
               props[family].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                graphics_support[device] = { true, family };
            }

            // If we don't have present support yet, check this queue family
            // against the surface we created earlier. getSurfaceSupportKHR()
            // actually just checks whether a given queue family can present
            // on a given surface. Poorly named function, but oh well.
            if(!present_support[device].first) {
                auto result =
                    vk_physical_dev.getSurfaceSupportKHR(family, surface);

                // Oblige Vulkan-Hpp and check its return values
                if(result.result != vk::Result::eSuccess) {
                    CONSOLE_CRITICAL("Failed to query surface support.");
                }
                if(result.value) {
                    present_support[device] = { true, family };
                }
            }
        }
    }

    for(uint32_t device_index = 0u;
        device_index < _available_devices.size();
        ++device_index)
    {
        const bool gfx = graphics_support[device_index].first;
        const uint32_t gfx_fam = graphics_support[device_index].second;

        const bool present = present_support[device_index].first;
        const uint32_t present_fam = present_support[device_index].second;

        if(gfx && present && gfx_fam != std::numeric_limits<uint32_t>::max() &&
           present_fam != std::numeric_limits<uint32_t>::max())
        {
            _physical_device      = _available_devices[device_index].device;
            _graphics_queue_index = graphics_support[device_index].second;
            _present_queue_index  = present_support[device_index].second;
            CONSOLE_TRACE(
                "Selected {}, queue {} for graphics and {} for present",
                _available_devices[device_index].name,
                _graphics_queue_index,
                _present_queue_index
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
void PhysicalDevice::_print_family_flags(const uint32_t family,
                             const vk::QueueFlags flags) const
{
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
}

// =============================================================================
PhysicalDevice::PhysicalDevice() :
    _physical_device { nullptr },
    _graphics_queue_index { std::numeric_limits<uint32_t>::max() },
    _present_queue_index  { std::numeric_limits<uint32_t>::max() }
{ }

} // namespace vkl