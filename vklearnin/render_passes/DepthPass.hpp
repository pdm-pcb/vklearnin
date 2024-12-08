#ifndef VKLEARNIN_RENDERPASSES_DEPTHPASS_HPP
#define VKLEARNIN_RENDERPASSES_DEPTHPASS_HPP

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

class DepthPass final {
public:
    DepthPass() = default;
    ~DepthPass() = default;

    DepthPass(DepthPass &&) = delete;
    DepthPass(DepthPass const &) = delete;

    DepthPass & operator=(DepthPass &&) = delete;
    DepthPass & operator=(DepthPass const &) = delete;

    bool create(vkSurface const &surface,
                vkPhysicalDevice const &physical_device,
                vkDevice const &device);
    bool destroy();

    void destroy_swapchain_resources();
    void create_swapchain_resources(vkSurface const &surface,
                                    vkPhysicalDevice const &physical_device,
                                    vkDevice const &device);

    void begin(vkFrameBuffer const &frame_buffer,
               std::span<vk::ClearValue const> const clear_values,
               vkCmdBuffer const &cmd_buffer);

    inline auto const & render_pass() const { return _render_pass; }
    inline auto const & depth_view() const { return _depth_view; }

private:
    vkRenderPass _render_pass;

    std::vector<vk::AttachmentDescription> _attachment_descriptions;

    std::vector<vk::AttachmentReference> _color_refs;
    vk::AttachmentReference              _depth_ref { };

    std::vector<vk::SubpassDescription> _subpass_descriptions;
    std::vector<vk::SubpassDependency>  _subpass_deps;

    vk::Rect2D _render_area { };

    vk::Format _color_format { vk::Format::eUndefined };
    vk::Format _depth_format { vk::Format::eUndefined };

    vkImage     _depth_buffer;
    vkImageView _depth_view;

    std::vector<std::array<vk::ImageView, 2u>> _views_by_frame_index;

    bool _find_depth_format(vkPhysicalDevice const &physical_device);

    void _init_attachments();
    void _init_subpasses();

    bool _create_depth_buffer(vkPhysicalDevice const &physical_device,
                              vkDevice const &device);
    void _destroy_depth_buffer();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERPASSES_DEPTHPASS_HPP