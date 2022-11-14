#ifndef VKLEARNIN_RENDERING_PHYSICALDEVICE_HPP
#define VKLEARNIN_RENDERING_PHYSICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class PhysicalDevice final {
public:
    struct DeviceProperties {
        std::string name  = "";
        size_t vram_bytes = 0;
        std::string driver_version = "";
        std::string vkapi_version  = "";
        vk::PhysicalDevice device = nullptr;
    };

    using DeviceList = std::vector<DeviceProperties>;

    static void init();

    inline static auto graphics_queue_index() { return _graphics_queue_index; }
    inline static auto present_queue_index()  { return _present_queue_index;  }
    inline static const auto & family_indices() { return _family_indices;     }
    inline static const auto & native()         { return _physical_device;    }

    PhysicalDevice() = delete;
    ~PhysicalDevice() = delete;

    PhysicalDevice(PhysicalDevice &&other) = delete;
    PhysicalDevice(const PhysicalDevice &other) = delete;

    PhysicalDevice & operator=(PhysicalDevice &&other) = delete;
    PhysicalDevice & operator=(const PhysicalDevice &other) = delete;

private:
    static DeviceList _available_devices;
    static vk::PhysicalDevice _physical_device;

    static uint32_t _graphics_queue_index;
    static uint32_t _present_queue_index;
    static std::vector<uint32_t> _family_indices;

    static void _select_device();

    static void _store_physical_device(
        const vk::PhysicalDevice &device,
        const vk::PhysicalDeviceProperties &properties,
        const vk::PhysicalDeviceMemoryProperties &memory,
        const vk::PhysicalDeviceDriverProperties &drivers
    );

    static void _print_family_flags(const uint32_t family,
                                    const vk::QueueFlags flags);
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_PHYSICALDEVICE_HPP