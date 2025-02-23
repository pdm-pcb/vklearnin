#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"

#include "vklearnin/vulkan/vkInstance.hpp"
#include "vklearnin/vulkan/swapchain/vkSurface.hpp"

namespace vkl {

std::vector<vkPhysicalDevice *> vkPhysicalDevice::_available_devices;
vkPhysicalDevice const * vkPhysicalDevice::_current_device { nullptr };

// =============================================================================
bool vkPhysicalDevice::populate_device_list(
    vkInstance const &instance,
    vkSurface const &surface,
    Features const &features,
    std::span<char const * const> const extensions)
{
    // Ask the instance for a list of devices
    auto const [result, devices] =
        instance.native().enumeratePhysicalDevices();

    if(result != vk::Result::eSuccess) {
        Log::error(
            "Failed to enumerate physical devices: '{}'",
             vk::to_string(result)
        );
        return false;
    }

    Log::trace(
        "Found {} {}",
        devices.size(),
        (devices.size() == 1 ? "device" : "devices")
    );

    // Run through the devices, and only store ones that have what we need
    for(auto const &device : devices) {
        auto *candidate = new vkPhysicalDevice(device);

        if(!candidate->_check_queue_families(surface)) {
            delete candidate;
            continue;
        }

        if(!candidate->_check_features(features)) {
            delete candidate;
            continue;
        }

        if(!candidate->_check_extensions(extensions)) {
            delete candidate;
            continue;
        }

        _available_devices.emplace_back(candidate);
    }

    if(_available_devices.empty()) {
        Log::critical("Could not find suitable physical device.");
        return false;
    }

    // Sort remaining devices by "performance"
    _sort_devices();

    return true;
}

// =============================================================================
bool vkPhysicalDevice::select_device(vk::PhysicalDeviceType const type) {
    for(auto const *device : _available_devices) {
        if(device->_type == type) {
            _current_device = device;
            Log::info("Selected {}", _current_device->_name);
            return true;
        }
    }

    Log::error(
        "Failed to select device of type {}",
        vk::to_string(type)
    );
    return false;
}

// =============================================================================
void vkPhysicalDevice::clear_device_list() {
    for(auto *device : _available_devices) {
        delete device;
    }

    _available_devices.clear();
}

// =============================================================================
vkPhysicalDevice const & vkPhysicalDevice::current_device() {
    if(_current_device == nullptr) {
        Log::critical("No physical device selected.");
    }
    return *_current_device;
}

// =============================================================================
vk::Format
vkPhysicalDevice::find_depth_format(std::span<vk::Format const> const formats)
const
{
    for(auto const format : formats) {
        auto props = _handle.getFormatProperties(format);
        if(props.optimalTilingFeatures &
           vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
            Log::trace(
                "{} selected depth format {}",
                _name,
                vk::to_string(format)
            );
            return format;
        }
    }

    Log::error("{} failed to find suitable depth format.", _name);
    return vk::Format::eUndefined;
}

// =============================================================================
vkPhysicalDevice::vkPhysicalDevice(vk::PhysicalDevice const handle) :
    _handle { handle }
{
    // Retrieve the basic properties of the card
    auto const &device_props = _handle.getProperties();
    _type = device_props.deviceType;
    _name = std::string(device_props.deviceName.data());
    _vkapi_version = fmt::format(
        "{}.{}.{}",
        VK_API_VERSION_MAJOR(device_props.apiVersion),
        VK_API_VERSION_MINOR(device_props.apiVersion),
        VK_API_VERSION_PATCH(device_props.apiVersion)
    );

    _max_aniso = device_props.limits.maxSamplerAnisotropy;

    _samples = device_props.limits.framebufferColorSampleCounts
               & device_props.limits.framebufferDepthSampleCounts;

    _get_max_samples();

    // And grab some of the sneakier features we're interested in
    _vram_bytes     = _get_vram_bytes(_handle);
    _driver_version = _get_driver_version(_handle);

    Log::trace(
        "\n    Device Name:    {}"
        "\n    Device Type:    {}"
        "\n    VRAM:           {} MB"
        "\n    Max aniso:      {}"
        "\n    Max samples:    {}"
        "\n    Driver Version: {}"
        "\n    Vulkan Version: {}",
        _name,
        vk::to_string(_type),
        _vram_bytes / 1000 / 1000,
        _max_aniso,
        vk::to_string(_max_samples),
        _driver_version,
        _vkapi_version
    );
}

// =============================================================================
void vkPhysicalDevice::_sort_devices() {
    // Sort the available devices by VRAM, favoring discrete GPUs
    std::sort(_available_devices.begin(), _available_devices.end(),
        [&](auto const &a, auto const &b)
        {
            if(a->_type == vk::PhysicalDeviceType::eDiscreteGpu &&
               b->_type != vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return true;
            }

            if(a->_type == b->_type) {
                return a->_vram_bytes > b->_vram_bytes;
            }

            return false;
        }
    );
}

// =============================================================================
uint64_t vkPhysicalDevice::_get_vram_bytes(vk::PhysicalDevice const &device) {
    auto const &memory_props = device.getMemoryProperties();
    size_t vram_bytes = 0u;
    for(uint32_t index = 0u; index < memory_props.memoryHeapCount; ++index) {
        auto const flags = memory_props.memoryHeaps[index].flags;

        if((flags | vk::MemoryHeapFlagBits::eDeviceLocal) == flags) {
            vram_bytes = memory_props.memoryHeaps[index].size;
            break;
        }
    }
    return vram_bytes;
}

// =============================================================================
std::string
vkPhysicalDevice::_get_driver_version(vk::PhysicalDevice const &device) {
    vk::PhysicalDeviceDriverProperties driver_props { };
    vk::PhysicalDeviceProperties2 physical_props2 {
        .pNext = &driver_props
    };
    device.getProperties2(&physical_props2);

    return std::string(driver_props.driverInfo.data());
}

// =============================================================================
void vkPhysicalDevice::_print_family_flags(uint32_t const family,
                                           vk::QueueFlags const flags)
{
    std::string flags_str = fmt::format("{}: ", family);
    flags_str.reserve(128);

    if(flags & vk::QueueFlagBits::eGraphics) {
        flags_str += "Graphics        ";
    }
    if(flags & vk::QueueFlagBits::eCompute) {
        flags_str += "Compute         ";
    }
    if(flags & vk::QueueFlagBits::eTransfer) {
        flags_str += "Transfer        ";
    }
    if(flags & vk::QueueFlagBits::eSparseBinding) {
        flags_str += "Sparse Binding  ";
    }
    if(flags & vk::QueueFlagBits::eProtected) {
        flags_str += "Protected       ";
    }

#ifdef VK_KHR_video_decode_queue
    if(flags & vk::QueueFlagBits::eVideoDecodeKHR) {
        flags_str += "Video Decode    ";
    }
#endif // VK_KHR_video_decode_queue

#ifdef VK_KHR_video_encode_queue
    if(flags & vk::QueueFlagBits::eVideoEncodeKHR) {
        flags_str += "Video Encode    ";
    }
#endif // VK_KHR_video_encode_queue

#ifdef VK_NV_optical_flow
    if(flags & vk::QueueFlagBits::eOpticalFlowNV) {
        flags_str += "Optical Flow    ";
    }
#endif // VK_NV_optical_flow

    if(flags_str.size() < 16) {
        flags_str += "Present Only";
    }

    Log::trace("  {}", flags_str);
}

// =============================================================================
bool vkPhysicalDevice::_check_queue_families(vkSurface const &surface) {
    auto const &families = _handle.getQueueFamilyProperties();

    for(uint32_t i = 0u; i < families.size(); ++i) {
        _print_family_flags(i, families[i].queueFlags);
    }

    bool found_unified_family = false;
    for(uint32_t i = 0u; i < families.size(); ++i) {
        // The first check is if this queue family supports graphics commands
        if(families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            auto const [result, present_support] =
                _handle.getSurfaceSupportKHR(i, surface.native());

            if(result != vk::Result::eSuccess) {
                Log::error(
                    "{} failed to get surface {} support for queue family "
                    "index {}: '{}'",
                    _name,
                    surface.native(),
                    i,
                    vk::to_string(result)
                );
                continue;
            }

            // And the second is if this device can present on the surface
            // we've been given
            if(present_support == vk::True) {
                if(families[i].queueFlags & vk::QueueFlagBits::eCompute) {
                    found_unified_family = true;
                    _cmd_queue_index = i;
                    _compute_queue_index = i;

                    Log::trace(
                        "{} queue family index {} supports graphics, present, "
                        "and compute.",
                        _name,
                        _cmd_queue_index
                    );

                    break;
                }
            }
        }
    }

    if(!found_unified_family) {
        Log::warn(
            "{} doesn't support a unified graphics and present queue.",
            _name
        );
    }

    return found_unified_family;
}

// =============================================================================
bool vkPhysicalDevice::_check_features(Features const &features) {
    // Copy over the requested features
    _features12 = vk::PhysicalDeviceVulkan12Features {
        .pNext = nullptr,
    };

    _features11 = vk::PhysicalDeviceVulkan11Features {
        .pNext = &_features12,
    };

    _features = vk::PhysicalDeviceFeatures2 {
        .pNext = &_features11,
        .features {
            .fillModeNonSolid = features.fill_mode_nonsolid,
            .samplerAnisotropy = features.sampler_anisotropy,
        }
    };

    auto **next_feature = &_features12.pNext;

    if(features.sync2) {
        _sync2.synchronization2 = vk::True;
        *next_feature = &_sync2;
        next_feature = &_sync2.pNext;
    }

    if(features.dynamic_rendering) {
        _dynamic_rendering.dynamicRendering = vk::True;
        *next_feature = &_dynamic_rendering;
        next_feature = &_dynamic_rendering.pNext;
    }

    // Build the structure chain we'll use to query the device
    vk::PhysicalDeviceVulkan12Features supported12 {
        .pNext = nullptr,
    };
    vk::PhysicalDeviceVulkan11Features supported11 {
        .pNext = &supported12,
    };
    vk::PhysicalDeviceFeatures2 supported {
        .pNext = &supported11,
    };

    vk::PhysicalDeviceSynchronization2FeaturesKHR sync2_supported { };
    vk::PhysicalDeviceDynamicRenderingFeaturesKHR dr_supported { };

    auto **next_supported = &supported12.pNext;

    if(features.sync2) {
        *next_supported = &sync2_supported;
        next_supported = &sync2_supported.pNext;
    }

    if(features.dynamic_rendering) {
        *next_supported = &dr_supported;
        next_supported = &dr_supported.pNext;
    }

    // Ask the device what we're dealing with
    _handle.getFeatures2(&supported);

    // Run through and check what we care about
    bool all_features_supported = true;

    // VK1.0 features ----------------------------------------------------------
    if(_features.features.fillModeNonSolid
       && supported.features.fillModeNonSolid)
    {
        Log::trace("{} supports fillModeNonSolid.", _name);
    }
    else if(_features.features.fillModeNonSolid) {
        Log::warn("{} does not support fillModeNonSolid.", _name);
        all_features_supported = false;
    }

    if(_features.features.samplerAnisotropy
       && supported.features.samplerAnisotropy)
    {
        Log::trace("{} supports samplerAnisotropy.", _name);
    }
    else if(_features.features.samplerAnisotropy) {
        Log::warn("{} does not samplerAnisotropy.", _name);
        all_features_supported = false;
    }

    // VK1.1 features ----------------------------------------------------------
    // ...

    // VK1.2 features ----------------------------------------------------------
    // ...

    // Extensions --------------------------------------------------------------
    if(_sync2.synchronization2 && sync2_supported.synchronization2) {
        Log::trace("{} supports synchronization2.", _name);
    }
    else if(_sync2.synchronization2) {
        Log::warn("{} does not support synchronization2.", _name);
        all_features_supported = false;
    }

    if(_dynamic_rendering.dynamicRendering
        && dr_supported.dynamicRendering)
    {
        Log::trace("{} supports dynamicRendering.", _name);
    }
    else if(_dynamic_rendering.dynamicRendering) {
        Log::warn("{} does not support dynamicRendering.", _name);
        all_features_supported = false;
    }

    return all_features_supported;
}

// =============================================================================
bool
vkPhysicalDevice::_check_extensions(std::span<char const * const> extensions) {
    // Get the list of supported extensions
    auto const [ result, supported_extensions ] =
        _handle.enumerateDeviceExtensionProperties();

    if(result != vk::Result::eSuccess) {
        Log::error(
            "{} failed to enumerate extensions: '{}'",
            _name,
            vk::to_string(result)
        );
        return false;
    }

    Log::trace(
        "Found {} extensions for {}",
        supported_extensions.size(),
        _name
    );

    // Run through the required list and the supported list to make sure the
    // latter contains all of the former
    bool all_extensions_supported = true;
    for(char const * const required : extensions) {
        bool extension_found = false;

        for(auto const &supported : supported_extensions) {
            if(::strcmp(required, supported.extensionName) == 0) {
                _extensions.push_back(supported);
                extension_found = true;
                Log::trace("{} supports '{}'", _name, required);
                break;
            }
        }

        if(extension_found == false) {
            Log::warn("{} does not support '{}'", _name, required);
            all_extensions_supported = false;
        }
    }

    return all_extensions_supported;
}

// =============================================================================
void vkPhysicalDevice::_get_max_samples() {
    if(_samples & vk::SampleCountFlagBits::e64) {
        _max_samples = vk::SampleCountFlagBits::e64;
    }
    else if(_samples & vk::SampleCountFlagBits::e32) {
        _max_samples = vk::SampleCountFlagBits::e32;
    }
    else if(_samples & vk::SampleCountFlagBits::e16) {
        _max_samples = vk::SampleCountFlagBits::e16;
    }
    else if(_samples & vk::SampleCountFlagBits::e8) {
        _max_samples = vk::SampleCountFlagBits::e8;
    }
    else if(_samples & vk::SampleCountFlagBits::e4) {
        _max_samples = vk::SampleCountFlagBits::e4;
    }
    else if(_samples & vk::SampleCountFlagBits::e2) {
        _max_samples = vk::SampleCountFlagBits::e2;
    }
    else if(_samples & vk::SampleCountFlagBits::e1) {
        _max_samples = vk::SampleCountFlagBits::e1;
    }
}

} // namespace vkl