#ifndef VKLEARNIN_RENDERPASSES_COLORDEPTHRESOLVEPASS_HPP
#define VKLEARNIN_RENDERPASSES_COLORDEPTHRESOLVEPASS_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/pipelines/vkRenderPass.hpp"
#include "vklearnin/vulkan/resources/vkImage.hpp"
#include "vklearnin/vulkan/resources/vkImageView.hpp"

namespace vkl {

class vkSurface;
class vkPhysicalDevice;
class vkDevice;
class vkFrameBuffer;
class vkCmdBuffer;
class vkImage;
class vkImageView;

class ColorDepthResolvePass final {
public:
    ColorDepthResolvePass() = default;
    ~ColorDepthResolvePass() = default;

    ColorDepthResolvePass(ColorDepthResolvePass &&) = delete;
    ColorDepthResolvePass(ColorDepthResolvePass const &) = delete;

    ColorDepthResolvePass & operator=(ColorDepthResolvePass &&) = delete;
    ColorDepthResolvePass & operator=(ColorDepthResolvePass const &) = delete;

    bool create(vkSurface const &surface,
                vkPhysicalDevice const &physical_device,
                vkDevice const &device,
                vk::SampleCountFlagBits const msaa_samples);
    bool destroy();

    void destroy_swapchain_resources();
    void create_swapchain_resources(vkSurface const &surface,
                                    vkPhysicalDevice const &physical_device,
                                    vkDevice const &device);

    void begin(vkFrameBuffer const &frame_buffer,
               std::span<vk::ClearValue const> const clear_values,
               vkCmdBuffer const &cmd_buffer);

    inline auto const & render_pass()      const { return _render_pass; }
    inline auto const & multisample_view() const { return _multisample_view; }
    inline auto const & depth_view()       const { return _depth_view; }

private:
    vkRenderPass _render_pass;

    std::vector<vk::AttachmentDescription> _attachment_descriptions;

    vk::AttachmentReference _multisample_ref { };
    vk::AttachmentReference _depth_ref       { };
    vk::AttachmentReference _resolve_ref     { };

    vk::SubpassDescription _subpass_desc { };
    std::vector<vk::SubpassDependency> _subpass_deps;

    vk::Rect2D _render_area { };

    vk::Format _color_format { vk::Format::eUndefined };
    vk::Format _depth_format { vk::Format::eUndefined };

    vk::SampleCountFlagBits _msaa_samples { };

    vkImage     _multisample_buffer;
    vkImageView _multisample_view;

    vkImage     _depth_buffer;
    vkImageView _depth_view;

    std::vector<std::array<vk::ImageView, 2u>> _views_by_frame_index;

    bool _find_depth_format(vkPhysicalDevice const &physical_device);

    void _init_attachments();
    void _init_subpasses();

    bool _create_multisample_buffer(vkPhysicalDevice const &physical_device,
                                    vkDevice const &device);
    void _destroy_multisample_buffer();

    bool _create_depth_buffer(vkPhysicalDevice const &physical_device,
                              vkDevice const &device);
    void _destroy_depth_buffer();
};

} // namespace vkl


#endif // VKLEARNIN_RENDERPASSES_COLORDEPTHRESOLVEPASS_HPP