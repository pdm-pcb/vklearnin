#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"

#include "vklearnin/vulkan/vkInstance.hpp"
#include "vklearnin/vulkan/swapchain/vkSurface.hpp"

namespace vkl {

std::vector<vkPhysicalDevice *> vkPhysicalDevice::_available_devices;
vkPhysicalDevice const * vkPhysicalDevice::_current_device { nullptr };

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

    _msaa_samples = device_props.limits.framebufferColorSampleCounts
                    & device_props.limits.framebufferDepthSampleCounts;

    _get_max_msaa_samples();

    // And grab some of the sneakier features we're interested in
    _vram_bytes     = _get_vram_bytes();
    _driver_version = _get_driver_version();

    Log::trace("\n    Device Name:    {}"
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
               vk::to_string(_max_msaa_samples),
               _driver_version,
               _vkapi_version);
}

// =============================================================================
bool vkPhysicalDevice::populate_device_list(
    vkInstance const &instance,
    vkSurface const &surface,
    Features const &features,
    std::span<char const * const> const extensions)
{
    // Ask the instance for a list of devices
    auto const devices = instance.native().enumeratePhysicalDevices();

    if(devices.empty()) {
        Log::error("Failed to enumerate physical devices.");
        return false;
    }

    Log::trace("Found {} devices", devices.size());

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
void vkPhysicalDevice::clear_device_list() {
    for(auto *device : _available_devices) {
        delete device;
    }

    _available_devices.clear();
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

    Log::error("Failed to select device of type {}", vk::to_string(type));
    return false;
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
            Log::trace("{} selected depth format {}",
                       _name,
                       vk::to_string(format));

            return format;
        }
    }

    Log::error("{} failed to find suitable depth format.", _name);
    return vk::Format::eUndefined;
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
void vkPhysicalDevice::_get_max_msaa_samples() {
    if(_msaa_samples & vk::SampleCountFlagBits::e64) {
        _max_msaa_samples = vk::SampleCountFlagBits::e64;
    }
    else if(_msaa_samples & vk::SampleCountFlagBits::e32) {
        _max_msaa_samples = vk::SampleCountFlagBits::e32;
    }
    else if(_msaa_samples & vk::SampleCountFlagBits::e16) {
        _max_msaa_samples = vk::SampleCountFlagBits::e16;
    }
    else if(_msaa_samples & vk::SampleCountFlagBits::e8) {
        _max_msaa_samples = vk::SampleCountFlagBits::e8;
    }
    else if(_msaa_samples & vk::SampleCountFlagBits::e4) {
        _max_msaa_samples = vk::SampleCountFlagBits::e4;
    }
    else if(_msaa_samples & vk::SampleCountFlagBits::e2) {
        _max_msaa_samples = vk::SampleCountFlagBits::e2;
    }
    else if(_msaa_samples & vk::SampleCountFlagBits::e1) {
        _max_msaa_samples = vk::SampleCountFlagBits::e1;
    }
}

// =============================================================================
uint64_t vkPhysicalDevice::_get_vram_bytes() {
    auto const &memory_props = _handle.getMemoryProperties();
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
std::string vkPhysicalDevice::_get_driver_version() {
    vk::StructureChain<vk::PhysicalDeviceProperties2,
                       vk::PhysicalDeviceDriverProperties> props;

    _handle.getProperties2(&props.get());

    auto const & driver_props = props.get<vk::PhysicalDeviceDriverProperties>();

    return std::string(driver_props.driverInfo.data());
}

// =============================================================================
bool vkPhysicalDevice::_check_queue_families(vkSurface const &surface) {
    auto const &families = _handle.getQueueFamilyProperties();

    for(uint32_t i = 0u; i < families.size(); ++i) {
        _print_family_flags(i, families[i].queueFlags);
    }

    for(uint32_t i = 0u; i < families.size(); ++i) {
        // The first check is if this queue family supports graphics commands
        if(families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            auto const present_support =
                _handle.getSurfaceSupportKHR(i, surface.native());

            if(!present_support) {
                Log::error("{} family index {} with surface {} does not "
                           "support present.",
                           _name,
                           i,
                           surface.native());
                continue;
            }

            _graphics_queue_index = i;
            Log::trace("{} selecting queue family index {} for graphics.",
                       _name,
                       _graphics_queue_index);
            return true;
        }
    }

    return false;
}

// =============================================================================
bool vkPhysicalDevice::_check_features(Features const &features) {
    // Populate the local chain with the requested features
    auto &features10 = _features.get();
    features10.features = vk::PhysicalDeviceFeatures {
        .fillModeNonSolid = features.fill_mode_nonsolid,
        .samplerAnisotropy = features.sampler_anisotropy,
    };

    auto &features13 = _features.get<vk::PhysicalDeviceVulkan13Features>();
    features13 = vk::PhysicalDeviceVulkan13Features {
        .synchronization2 = features.sync2,
        .dynamicRendering = features.dynamic_rendering
    };

    // Ask the device if we can have what we want
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan13Features> supported;
    _handle.getFeatures2(&(supported.get()));

    // Run through and check what we care about
    bool all_features_supported = true;

    // VK1.0 features ----------------------------------------------------------
    auto const &supported10 = supported.get();
    if(features.fill_mode_nonsolid) {
        if(supported10.features.fillModeNonSolid) {
            Log::trace("{} supports fillModeNonSolid.", _name);
        }
        else {
            Log::warn("{} does not support fillModeNonSolid.", _name);
            all_features_supported = false;
        }
    }

    if(features.sampler_anisotropy) {
        if(supported10.features.samplerAnisotropy) {
            Log::trace("{} supports samplerAnisotropy.", _name);
        }
        else {
            Log::warn("{} does not support samplerAnisotropy.", _name);
            all_features_supported = false;
        }
    }

    // VK1.3 features ----------------------------------------------------------
    auto const &supported13 = supported.get<vk::PhysicalDeviceVulkan13Features>();
    if(features.sync2) {
        if(supported13.synchronization2) {
            Log::trace("{} supports synchronization2.", _name);
        }
        else {
            Log::warn("{} does not support synchronization2.", _name);
            all_features_supported = false;
        }
    }

    if(features.dynamic_rendering) {
        if(supported13.dynamicRendering) {
            Log::trace("{} supports dynamicRendering.", _name);
        }
        else {
            Log::warn("{} does not support dynamicRendering.", _name);
            all_features_supported = false;
        }
    }

    return all_features_supported;
}

// =============================================================================
bool
vkPhysicalDevice::_check_extensions(std::span<char const * const> extensions) {
    // Get the list of supported extensions
    auto const supported_extensions =
        _handle.enumerateDeviceExtensionProperties();

    if(supported_extensions.empty()) {
        Log::error("{} failed to enumerate extensions.", _name);
        return false;
    }

    Log::trace("{} found {} extensions", _name, supported_extensions.size());

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

#ifdef VK_ARM_data_graph
    if(flags & vk::QueueFlagBits::eDataGraphARM) {
        flags_str += "ARM Data Graph  ";
    }
#endif // VK_ARM_data_graph

    if(flags_str.size() < 16) {
        flags_str += "Present Only";
    }

    Log::trace("  {}", flags_str);
}

} // namespace vkl