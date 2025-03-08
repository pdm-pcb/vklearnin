#ifndef VKLEARNIN_RENDERING_DYNAMIC_DEPTHDYNAMIC_HPP
#define VKLEARNIN_RENDERING_DYNAMIC_DEPTHDYNAMIC_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/resources/vkImage.hpp"
#include "vklearnin/vulkan/resources/vkImageView.hpp"

namespace vkl {

class vkSurface;
class vkPhysicalDevice;
class vkDevice;

class DepthDynamic final {
public:
    DepthDynamic() = default;
    ~DepthDynamic() = default;

    DepthDynamic(DepthDynamic &&) = delete;
    DepthDynamic(DepthDynamic const &) = delete;

    DepthDynamic & operator=(DepthDynamic &&) = delete;
    DepthDynamic & operator=(DepthDynamic const &) = delete;

    void init(vkSurface const &surface,
              std::span<vk::ClearValue const> const clear_values,
              vk::Format const depth_format,
              vkPhysicalDevice const &physical_device,
              vkDevice const &device);

    void shutdown();

    void update_render_area(vkSurface const &surface);

    void destroy_swapchain_resources();
    void create_swapchain_resources(
        vkSurface const &surface,
        std::span<vk::ClearValue const> const clear_values,
        vk::Format const depth_format,
        vkPhysicalDevice const &physical_device,
        vkDevice const &device);

    vk::RenderingInfoKHR const & rendering_info(vk::ImageView const &view,
                                                vk::ImageLayout const &layout);

    inline auto & depth_buffer() { return _depth_buffer; }

    inline auto const & pipeline_create_info() const {
        return _pipeline_create_info;
    }

private:
    std::vector<vk::Format> _color_attachment_formats;
    vk::Format _depth_attachment_format { vk::Format::eUndefined };

    std::vector<vk::RenderingAttachmentInfoKHR> _color_attachments;
    vk::RenderingAttachmentInfoKHR _depth_attachment { };

    vkImage     _depth_buffer;
    vkImageView _depth_view;

    vk::RenderingInfoKHR _rendering_info { };
    vk::PipelineRenderingCreateInfoKHR _pipeline_create_info { };

    void _init_attachments(std::span<vk::ClearValue const> const clear_values);
    void _init_rendering_info(vkSurface const &surface);
    void _init_pipeline_create_info();

    bool _create_depth_buffer(vkSurface const &surface,
                              vkPhysicalDevice const &physical_device,
                              vkDevice const &device);
    void _destroy_depth_buffer();

    void _reset_object();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DYNAMIC_DEPTHDYNAMIC_HPP