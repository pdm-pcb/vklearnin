#ifndef VKLEARNIN_RENDERING_DYNAMIC_MSAADYNAMIC_HPP
#define VKLEARNIN_RENDERING_DYNAMIC_MSAADYNAMIC_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/resources/vkImage.hpp"
#include "vklearnin/vulkan/resources/vkImageView.hpp"

namespace vkl {

class vkSurface;
class vkPhysicalDevice;
class vkDevice;

class MSAADynamic final {
public:
    MSAADynamic() = default;
    ~MSAADynamic() = default;

    MSAADynamic(MSAADynamic &&) = delete;
    MSAADynamic(MSAADynamic const &) = delete;

    MSAADynamic & operator=(MSAADynamic &&) = delete;
    MSAADynamic & operator=(MSAADynamic const &) = delete;

    void init(vkSurface const &surface,
              std::span<vk::ClearValue const> const clear_values,
              vk::Format const depth_format,
              vk::SampleCountFlagBits const msaa_sample_count,
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

    inline auto & depth_buffer()       { return _depth_buffer; }
    inline auto & multisample_buffer() { return _multisample_buffer; }

    inline auto const & pipeline_create_info() const {
        return _pipeline_create_info;
    }

private:
    std::vector<vk::Format> _resolve_attachment_formats;
    vk::Format _depth_attachment_format       { vk::Format::eUndefined };
    vk::Format _multisample_attachment_format { vk::Format::eUndefined };

    std::vector<vk::RenderingAttachmentInfoKHR> _resolve_attachments;
    vk::RenderingAttachmentInfoKHR _depth_attachment { };
    vk::RenderingAttachmentInfoKHR _multisample_attachment { };

    vkImage     _depth_buffer;
    vkImageView _depth_view;

    vkImage     _multisample_buffer;
    vkImageView _multisample_view;

    vk::SampleCountFlagBits _msaa_sample_count { vk::SampleCountFlagBits::e1 };

    vk::RenderingInfoKHR _rendering_info { };
    vk::PipelineRenderingCreateInfoKHR _pipeline_create_info { };

    void _init_attachments(std::span<vk::ClearValue const> const clear_values);
    void _init_rendering_info(vkSurface const &surface);
    void _init_pipeline_create_info();

    bool _create_depth_buffer(vkSurface const &surface,
                              vkPhysicalDevice const &physical_device,
                              vkDevice const &device);
    void _destroy_depth_buffer();

    bool _create_multisample_buffer(vkSurface const &surface,
                                    vkPhysicalDevice const &physical_device,
                                    vkDevice const &device);
    void _destroy_multisample_buffer();

    void _reset_object();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DYNAMIC_MSAADYNAMIC_HPP