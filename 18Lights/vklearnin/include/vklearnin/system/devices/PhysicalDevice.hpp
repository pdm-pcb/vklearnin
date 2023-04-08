#ifndef VKLEARNIN_SYSTEM_DEVICES_PHYSICALDEVICE_HPP
#define VKLEARNIN_SYSTEM_DEVICES_PHYSICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class PhysicalDevice final {
public:
    enum class Features {
        FILL_MODE_NONSOLID,
        SAMPLER_ANISOTROPY,
        DYNAMIC_RENDERING,
        NONUNIFORM_DESCRIPTOR_INDEXING,
    };

    static void query_devices(
        std::vector<std::string_view> const & required_extensions,
        std::vector<Features> const & required_features
    );
    static void select_device();

    inline static auto queue_index() { return _queue_index; }

    inline static auto const & native()       { return _physical_device;    }
    inline static auto const & memory_props() { return _memory_properties;  }
    inline static auto const & features()     { return _enabled_features;   }
    inline static auto const & extensions()   { return _enabled_extensions; }

    inline static void * features_chain() { return _features_chain; }

    inline static auto depth_format() { return _depth_format; }

    PhysicalDevice() = delete;

private:
    struct DeviceProps {
        std::string name;
        size_t vram_bytes = 0;
        uint8_t max_samples = 0u;
        float max_aniso = 0.0f;
        std::string driver_version;
        std::string vkapi_version;
        vk::PhysicalDevice device = nullptr;
        vk::PhysicalDeviceType type = vk::PhysicalDeviceType::eOther;
        vk::PhysicalDeviceMemoryProperties memory { };
    };

    using DeviceList = std::vector<DeviceProps>;
    static DeviceList _available_devices;

    static vk::PhysicalDevice _physical_device;

    static uint32_t _queue_index;

    static vk::PhysicalDeviceMemoryProperties _memory_properties;
    static vk::PhysicalDeviceFeatures         _enabled_features;
    static std::vector<char const *>          _enabled_extensions;

    static vk::PhysicalDeviceDynamicRenderingFeatures
        _dynamic_rendering_features;

    static vk::PhysicalDeviceDescriptorIndexingFeatures
        _descriptor_indexing_features;

    static void *_features_chain;

    static vk::Format _depth_format;

    static bool _check_features(
        vk::PhysicalDeviceFeatures2 const &supported_features,
        std::vector<Features> const &required_features
    );

    static bool _check_extensions(
        std::vector<vk::ExtensionProperties> const &supported_extensions,
        std::vector<std::string_view> const &required_extensions
    );

    static bool _check_depth_format(vk::PhysicalDevice const &device);

    static void _store_physical_device(
        const vk::PhysicalDevice &device,
        const vk::PhysicalDeviceProperties &properties,
        const vk::PhysicalDeviceMemoryProperties &memory,
        const vk::PhysicalDeviceDriverProperties &drivers
    );

    static void _print_family_flags(uint32_t const family,
                                    const vk::QueueFlags flags);
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_DEVICES_PHYSICALDEVICE_HPP