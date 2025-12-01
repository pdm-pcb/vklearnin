#ifndef VKLEARNIN_VULKAN_RESOURCES_VKIMAGE_HPP
#define VKLEARNIN_VULKAN_RESOURCES_VKIMAGE_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

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
                vk::Extent3D const &extent);

    // For reading texture data from a file
    bool create(std::string_view const file_name,
                Details const &details,
                vkDevice const &device);

    // For render targets, eg color buffer
    bool create(vk::Extent2D const &extent,
                vk::Format const format,
                Details const &details,
                vkDevice const &device);

    bool destroy();

    struct TransitionDetails {
        vk::ImageLayout const old_layout = vk::ImageLayout::eUndefined;
        vk::ImageLayout const new_layout = vk::ImageLayout::eUndefined;

        vk::ImageAspectFlags const aspect_flags { 0u };

        vk::PipelineStageFlags src_stage { 0u };
        vk::PipelineStageFlags dst_stage { 0u };

        vk::AccessFlags src_access { 0u };
        vk::AccessFlags dst_access { 0u };

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

    vk::Format           _format { vk::Format::eUndefined };
    vk::Extent3D         _extent { };
    vk::ImageAspectFlags _aspect_flags { };
    vk::ImageLayout      _layout { vk::ImageLayout::eUndefined };

    uint32_t _array_layers = 1u;
    uint32_t _mip_levels = 1u;

    vk::DeviceSize _size_bytes = 0u;
    void *_raw_data { nullptr };

    vkDevice const *_device { nullptr };

    void * _load_from_file(std::string_view const file_name);
    void _calc_mip_levels();
    bool _allocate(vk::MemoryPropertyFlags const memory_flags);
    bool _send_to_device();
    void _generate_mipmaps(vkCmdBuffer const &cmd_buffer,
                           vk::Filter const filter);

    uint32_t _memory_type_index(vk::MemoryPropertyFlags const flags,
                                vk::MemoryRequirements const &reqs);
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_RESOURCES_VKIMAGE_HPP