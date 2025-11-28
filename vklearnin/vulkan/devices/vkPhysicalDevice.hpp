#ifndef VKLEARNIN_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP
#define VKLEARNIN_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkInstance;
class vkSurface;

class vkPhysicalDevice final {
public:
    vkPhysicalDevice() = delete;

    vkPhysicalDevice(vkPhysicalDevice &&) = delete;
    vkPhysicalDevice(vkPhysicalDevice const &) = delete;

    vkPhysicalDevice & operator=(vkPhysicalDevice &&) = delete;
    vkPhysicalDevice & operator=(vkPhysicalDevice const &) = delete;

    struct Features {
        // VK1.0 features
        bool fill_mode_nonsolid = false;
        bool sampler_anisotropy = false;

        // VK1.1 features
        // ...

        // VK1.2 features
        // ...

        // VK1.3 features
        bool sync2 = false;
        bool dynamic_rendering  = false;
    };

    [[nodiscard]] static bool populate_device_list(
        vkInstance const &instance,
        vkSurface const &surface,
        Features const &features,
        std::span<char const * const> const extensions
    );

    static void clear_device_list();

    [[nodiscard]] static bool select_device(vk::PhysicalDeviceType const type);
    [[nodiscard]] static vkPhysicalDevice const & current_device();

    [[nodiscard]] vk::Format
    find_depth_format(std::span<vk::Format const> const formats) const;

    [[nodiscard]] inline auto const & native()         const { return _handle; }
    [[nodiscard]] inline std::string_view const name() const { return _name; }
    [[nodiscard]] inline auto type()                   const { return _type; }
    [[nodiscard]] inline auto vram_bytes()             const { return _vram_bytes; }
    [[nodiscard]] inline auto graphics_queue_index()   const { return _graphics_queue_index; }
    [[nodiscard]] inline auto const & features()       const { return _features.get<vk::PhysicalDeviceFeatures2>(); }
    [[nodiscard]] inline auto const & extensions()     const { return _extensions; }
    [[nodiscard]] inline auto max_aniso()              const { return _max_aniso; }
    [[nodiscard]] inline auto max_msaa_samples()       const { return _max_msaa_samples; }

private:
    explicit vkPhysicalDevice(vk::PhysicalDevice const handle);
    ~vkPhysicalDevice() = default;

    vk::PhysicalDevice _handle;

    vk::PhysicalDeviceType _type { };
    std::string  _name;
    std::string  _vkapi_version;
    uint64_t     _vram_bytes     { 0u };
    std::string  _driver_version;

    uint32_t _graphics_queue_index { std::numeric_limits<uint32_t>::max()};

    float _max_aniso { 0.0f };

    vk::SampleCountFlags    _msaa_samples { };
    vk::SampleCountFlagBits _max_msaa_samples { };

    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan13Features> _features { };

    std::vector<vk::ExtensionProperties> _extensions;

    static std::vector<vkPhysicalDevice *> _available_devices;
    static vkPhysicalDevice const *_current_device;

    static void _sort_devices();

    void _get_max_msaa_samples();
    [[nodiscard]] uint64_t _get_vram_bytes();
    [[nodiscard]] std::string _get_driver_version();

    [[nodiscard]] bool _check_queue_families(vkSurface const &surface);
    [[nodiscard]] bool _check_features(Features const &features);
    [[nodiscard]] bool _check_extensions(std::span<char const * const> extensions);

    static void _print_family_flags(uint32_t const family,
                                    vk::QueueFlags const flags);

};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP