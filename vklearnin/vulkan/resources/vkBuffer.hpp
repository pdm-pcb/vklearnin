#ifndef VKLEARNIN_VULKAN_RESOURCES_VKBUFFER_HPP
#define VKLEARNIN_VULKAN_RESOURCES_VKBUFFER_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkDevice;
class vkCmdPool;
class vkQueue;
class vkPhysicalDevice;

class vkBuffer final {
public:
    vkBuffer() = default;
    ~vkBuffer() = default;

    vkBuffer(vkBuffer &&other);
    vkBuffer(vkBuffer const &) = delete;

    vkBuffer & operator=(vkBuffer &&) = delete;
    vkBuffer & operator=(vkBuffer const &) = delete;

    bool create(vk::DeviceSize size_bytes,
                vk::BufferUsageFlags const usage_flags,
                vkPhysicalDevice const &physical_device,
                vkDevice const &device);
    bool destroy();

    bool allocate(vk::MemoryPropertyFlags const flags);
    bool free();

    bool fill_buffer(void const *data) const;
    bool send_to_device(void const *data,
                        vkCmdPool const &cmd_pool,
                        vkQueue const &queue) const;

    inline auto const & native() const { return _handle; }

private:
    vk::Buffer       _handle        { nullptr };
    vk::DeviceMemory _memory_handle { nullptr };
    vk::DeviceSize   _size_bytes    { 0u };

    vkPhysicalDevice const *_physical_device { nullptr };
    vkDevice         const *_device { nullptr };

    static uint32_t _get_memory_type_index(
        vk::PhysicalDeviceMemoryProperties const &properties,
        vk::MemoryPropertyFlags const flags,
        vk::MemoryRequirements const &reqs
    );
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_RESOURCES_VKBUFFER_HPP