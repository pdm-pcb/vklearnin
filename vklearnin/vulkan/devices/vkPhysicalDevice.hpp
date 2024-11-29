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

    static bool populate_device_list(
        vkInstance const &instance,
        vkSurface const &surface,
        vk::PhysicalDeviceFeatures2 const &features,
        std::span<char const * const> const extensions
    );

    static bool select_device(vk::PhysicalDeviceType const type);

    static void clear_device_list();

    static vkPhysicalDevice const & current_device();

    inline auto const & native()            const { return _handle; }
    inline std::string_view const name()    const { return _name; }
    inline auto type()                      const { return _type; }
    inline auto vram_bytes()                const { return _vram_bytes; }
    inline auto cmd_queue_index()      const { return _cmd_queue_index; }
    inline auto compute_queue_index()       const { return _compute_queue_index; }
    inline auto const & features()          const { return _features; }
    inline auto const & extensions()        const { return _extensions; }
    inline auto max_aniso()                 const { return _max_aniso; }
    inline auto const & max_samples()       const { return _max_samples; }

private:
    explicit vkPhysicalDevice(vk::PhysicalDevice const handle);
    ~vkPhysicalDevice() = default;

    vk::PhysicalDevice _handle;

    vk::PhysicalDeviceType _type { };
    std::string  _name;
    std::string  _vkapi_version;
    uint64_t     _vram_bytes     { 0u };
    std::string  _driver_version;

    uint32_t _cmd_queue_index { std::numeric_limits<uint32_t>::max()};
    uint32_t _compute_queue_index  { std::numeric_limits<uint32_t>::max()};

    float _max_aniso { 0.0f };

    vk::SampleCountFlags    _samples     { };
    vk::SampleCountFlagBits _max_samples { };

    vk::PhysicalDeviceFeatures2 _features   { };

    std::vector<vk::ExtensionProperties> _extensions;

    static std::vector<vkPhysicalDevice *> _available_devices;
    static vkPhysicalDevice const *_current_device;

    static void _sort_devices();

    static uint64_t _get_vram_bytes(vk::PhysicalDevice const &device);
    static std::string _get_driver_version(vk::PhysicalDevice const &device);
    static void _print_family_flags(uint32_t const family,
                                    vk::QueueFlags const flags);

    bool _check_queue_families(vkSurface const &surface);
    bool _check_features(vk::PhysicalDeviceFeatures2 const &features);
    bool _check_extensions(std::span<char const * const> extensions);

    void _get_max_samples();
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP