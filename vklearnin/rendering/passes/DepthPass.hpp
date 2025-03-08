#ifndef VKLEARNIN_RENDERING_PASSES_DEPTHPASS_HPP
#define VKLEARNIN_RENDERING_PASSES_DEPTHPASS_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/pipelines/vkRenderPass.hpp"
#include "vklearnin/vulkan/resources/vkImage.hpp"
#include "vklearnin/vulkan/resources/vkImageView.hpp"

namespace vkl {

class vkSurface;
class vkPhysicalDevice;
class vkDevice;

class DepthPass final {
public:
    DepthPass() = default;
    ~DepthPass() = default;

    DepthPass(DepthPass &&) = delete;
    DepthPass(DepthPass const &) = delete;

    DepthPass & operator=(DepthPass &&) = delete;
    DepthPass & operator=(DepthPass const &) = delete;

    bool create(vkSurface const &surface,
                std::span<vk::ClearValue const> const clear_values,
                vk::Format const depth_format,
                vkPhysicalDevice const &physical_device,
                vkDevice const &device);
    bool destroy();

    void update_render_area(vkSurface const &surface);

    void destroy_swapchain_resources();
    void create_swapchain_resources(vkSurface const &surface,
                                    vk::Format const depth_format,
                                    vkPhysicalDevice const &physical_device,
                                    vkDevice const &device);

    inline auto & begin_info() { return _begin_info; }

    inline auto const & render_pass() const { return _render_pass; }
    inline auto const & depth_view() const { return _depth_view; }

private:
    vkRenderPass _render_pass;

    vk::RenderPassBeginInfo _begin_info { };

    std::vector<vk::AttachmentDescription> _attachment_descriptions;

    std::vector<vk::AttachmentReference> _color_refs;
    vk::AttachmentReference              _depth_ref { };

    std::vector<vk::SubpassDescription> _subpass_descriptions;
    std::vector<vk::SubpassDependency>  _subpass_deps;

    vk::Format _color_format = vk::Format::eUndefined;
    vk::Format _depth_format = vk::Format::eUndefined;

    vkImage     _depth_buffer;
    vkImageView _depth_view;

    void _init_attachments();
    void _init_subpasses();

    bool _create_depth_buffer(vkSurface const &surface,
                              vkPhysicalDevice const &physical_device,
                              vkDevice const &device);
    void _destroy_depth_buffer();

    void _reset_object();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_PASSES_DEPTHPASS_HPP