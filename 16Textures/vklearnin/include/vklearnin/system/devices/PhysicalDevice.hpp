#ifndef VKLEARNIN_SYSTEM_DEVICES_PHYSICALDEVICE_HPP
#define VKLEARNIN_SYSTEM_DEVICES_PHYSICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class PhysicalDevice final {
public:
    static void query_devices();
    static void select_device();

    inline static auto queue_index()          { return _queue_index;       }
    inline static auto const& native()       { return _physical_device;   }
    inline static auto const& memory_props() { return _memory_properties; }

    PhysicalDevice() = delete;

private:
    struct DeviceProps {
        std::string name;
        size_t vram_bytes = 0;
        uint8_t max_samples = 0u;
        std::string driver_version;
        std::string vkapi_version;
        vk::PhysicalDevice device = nullptr;
        vk::PhysicalDeviceType type = vk::PhysicalDeviceType::eOther;
        vk::PhysicalDeviceMemoryProperties memory { };
    };
    using DeviceList = std::vector<DeviceProps>;

    static DeviceList         _available_devices;
    static vk::PhysicalDevice _physical_device;

    static uint32_t _queue_index;

    static vk::PhysicalDeviceMemoryProperties _memory_properties;

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