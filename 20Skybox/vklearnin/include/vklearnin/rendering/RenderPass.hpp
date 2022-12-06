#ifndef VKLEARNIN_RENDERING_RENDERPASS_HPP
#define VKLEARNIN_RENDERING_RENDERPASS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/Framebuffer.hpp"
#include "vklearnin/shaders/ImageObject.hpp"

namespace vkl {

class Swapchain;

class RenderPass final {
public:
    void create_framebuffers();
    void destroy_framebuffers();

    void create();
    void destroy();

    inline const auto & native() const { return _render_pass; }
    inline const auto & framebuffer(const uint32_t index) const {
        return _framebuffers[index];
    }

    explicit RenderPass(const Swapchain &swapchain);
    ~RenderPass() = default;
    RenderPass() = delete;

    RenderPass(RenderPass &&other) = delete;
    RenderPass(const RenderPass &other);

    RenderPass & operator=(RenderPass &&other) = delete;
    RenderPass & operator=(const RenderPass &other) = delete;

private:
    std::vector<vk::AttachmentDescription> _attachments;
    std::vector<vk::SubpassDescription>    _subpasses;
    std::vector<vk::SubpassDependency>     _subpass_dependencies;
    std::vector<vk::AttachmentReference>   _color_attachments;
    vk::AttachmentReference                _depth_attachment;
    std::vector<vk::AttachmentReference>   _resolve_attachments;

    vk::SampleCountFlagBits _sample_count_flags;

    ImageObject _color_buffer;
    ImageObject _depth_buffer;

    std::vector<Framebuffer> _framebuffers;

    vk::RenderPass _render_pass;

    const Swapchain &_swapchain;

    void _default_attachments();
    void _default_subpasses();
    void _default_subpass_dependencies();
    void _init_depth_buffer();
    void _init_color_buffer();
    vk::Format _find_depth_buffer_format();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERPASS_HPP