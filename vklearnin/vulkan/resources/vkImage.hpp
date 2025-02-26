#ifndef VKLEARNIN_VULKAN_RESOURCES_VKIMAGE_HPP
#define VKLEARNIN_VULKAN_RESOURCES_VKIMAGE_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkPhysicalDevice;
class vkDevice;
class vkCmdBuffer;

class vkImage final {
public:
    vkImage() = default;
    ~vkImage() = default;

    vkImage(vkImage &&other);
    vkImage(vkImage const &) = delete;

    vkImage & operator=(vkImage &&) = delete;
    vkImage & operator=(vkImage const &) = delete;

    struct Details final {
        vk::ImageType type = { };
        vk::SampleCountFlagBits samples = { };
        vk::ImageUsageFlags usage_flags = { };
        vk::MemoryPropertyFlags memory_flags = { };
        vk::ImageAspectFlags aspect_flags = { };
        bool generate_mips = false;
        uint32_t array_layers = 1u;
    };

    // For swapchain images
    bool create(vk::Image const &handle,
                vk::Format const format,
                vk::Extent3D const &extent,
                vkDevice const &device,
                std::string_view const debug_name);

    // For reading texture data from a file
    bool create(std::string_view const file_name,
                Details const &details,
                vkPhysicalDevice const &physical_device,
                vkDevice const &device,
                std::string_view const debug_name);

    // For render targets, eg color buffer
    bool create(vk::Extent2D const &extent,
                vk::Format const format,
                Details const &details,
                vkPhysicalDevice const &physical_device,
                vkDevice const &device,
                std::string_view const debug_name);

    bool destroy();

    struct TransitionDetails {
        vk::ImageLayout const old_layout = vk::ImageLayout::eUndefined;
        vk::ImageLayout const new_layout = vk::ImageLayout::eUndefined;

        vk::ImageAspectFlags const aspect_flags { 0u };

        uint32_t const base_mip_level = 0u;
        uint32_t const mip_level_count = 1u;
        uint32_t const base_array_layer = 0u;
        uint32_t const array_layer_count = 1u;
    };

    void transition_layout(vkCmdBuffer const &cmd_buffer,
                           TransitionDetails const &details);

    inline auto const & native() const { return _handle; }
    inline auto format() const { return _format; }
    inline auto layout() const { return _layout; }
    inline auto const & aspect_flags() const { return _aspect_flags; }

private:
    vk::Image        _handle        { nullptr };
    vk::DeviceMemory _memory_handle { nullptr };

    vkPhysicalDevice const *_physical_device { nullptr };
    vkDevice const *_device { nullptr };

    vk::Format _format { vk::Format::eUndefined };
    vk::Extent3D _extent { };
    vk::ImageAspectFlags _aspect_flags { };

    vk::ImageLayout _layout { vk::ImageLayout::eUndefined };

    uint32_t _array_layers = 1u;
    uint32_t _mip_levels = 1u;

    vk::DeviceSize _size_bytes = 0u;
    void *_raw_data { nullptr };

    void * _load_from_file(std::string_view const file_name);
    void _calc_mip_levels();
    bool _allocate(vk::MemoryPropertyFlags const memory_flags);
    bool _send_to_device();
    void _generate_mipmaps(vkCmdBuffer const &cmd_buffer,
                           vk::Filter const filter);

    static uint32_t _memory_type_index(
        vk::PhysicalDeviceMemoryProperties const &memory_props,
        vk::MemoryPropertyFlags const flags,
        vk::MemoryRequirements const reqs
    );

#ifdef VKL_DEBUG
    std::string _debug_name { "UNNAMED IMAGE" };
#endif // VKL_DEBUG
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_RESOURCES_VKIMAGE_HPP