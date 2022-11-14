#ifndef VKLEARNIN_RENDERING_PHYSICALDEVICE_HPP
#define VKLEARNIN_RENDERING_PHYSICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class PhysicalDevice final {
public:
    void init(const vk::Instance &graphics_instance);
    void select_device(const vk::SurfaceKHR &surface);

    uint32_t graphics_queue_index() const { return _graphics_queue_index; }
    uint32_t present_queue_index()  const { return _present_queue_index;  }
    std::set<uint32_t> family_indices() const {
        return std::set<uint32_t> {
            _graphics_queue_index,
            _present_queue_index
        };
    }
    const vk::PhysicalDevice & native() const { return _physical_device;  }

    PhysicalDevice();
    ~PhysicalDevice() = default;

    PhysicalDevice(PhysicalDevice &&other) = delete;
    PhysicalDevice(const PhysicalDevice &other) = delete;

    PhysicalDevice & operator=(PhysicalDevice &&other) = delete;
    PhysicalDevice & operator=(const PhysicalDevice &other) = delete;

private:
    struct DeviceProperties {
        std::string name  = "";
        size_t vram_bytes = 0;
        std::string driver_version = "";
        std::string vkapi_version  = "";
        vk::PhysicalDevice device = nullptr;
    };

    std::vector<DeviceProperties> _available_devices;
    vk::PhysicalDevice _physical_device;

    uint32_t _graphics_queue_index;
    uint32_t _present_queue_index;

    void _store_physical_device(
        const vk::PhysicalDevice &device,
        const vk::PhysicalDeviceProperties &properties,
        const vk::PhysicalDeviceMemoryProperties &memory,
        const vk::PhysicalDeviceDriverProperties &drivers
    );

    void _print_family_flags(const uint32_t family,
                             const vk::QueueFlags flags) const;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_PHYSICALDEVICE_HPP