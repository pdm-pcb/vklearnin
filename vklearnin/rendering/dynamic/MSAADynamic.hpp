/*

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
              vkPhysicalDevice const &physical_device,
              vkDevice const &device);

    void shutdown();

    void update_render_area(vkSurface const &surface);

    vk::RenderingInfoKHR const & rendering_info(vk::ImageView const &view,
                                                vk::ImageLayout const &layout);

    inline auto & depth_buffer() { return _depth_buffer; }

    inline auto const & pipeline_create_info() const {
        return _pipeline_create_info;
    }

private:
    std::vector<vk::RenderingAttachmentInfoKHR> _color_attachments;
    std::vector<vk::Format> _color_attachment_formats;

    vk::RenderingInfoKHR _rendering_info { };
    vk::PipelineRenderingCreateInfoKHR _pipeline_create_info { };

    vk::RenderingAttachmentInfoKHR _depth_attachment { };

    vk::Format  _depth_format { vk::Format::eUndefined };
    vkImage     _depth_buffer;
    vkImageView _depth_view;

    bool _create_depth_buffer(vkSurface const &surface,
                              vkPhysicalDevice const &physical_device,
                              vkDevice const &device);
    void _destroy_depth_buffer();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DYNAMIC_MSAADYNAMIC_HPP

*/