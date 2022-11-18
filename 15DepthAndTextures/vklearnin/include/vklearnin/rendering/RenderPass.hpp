#ifndef VKLEARNIN_RENDERING_RENDERPASS_HPP
#define VKLEARNIN_RENDERING_RENDERPASS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/Framebuffer.hpp"

namespace vkl {

class Swapchain;

class RenderPass final {
public:
    void create_framebuffers(const Swapchain &swapchain);
    void destroy_framebuffers();

    void create(const Swapchain &swapchain);
    void destroy();

    inline const auto & native() const { return _render_pass; }
    inline const auto & framebuffer(const uint32_t index) const {
        return _framebuffers[index];
    }

    RenderPass();
    ~RenderPass() = default;

    RenderPass(RenderPass &&other) = delete;
    RenderPass(const RenderPass &other);

    RenderPass & operator=(RenderPass &&other) = delete;
    RenderPass & operator=(const RenderPass &other) = delete;

private:
    std::vector<vk::AttachmentDescription> _attachments;
    std::vector<vk::AttachmentReference> _color_attachments;
    std::vector<vk::SubpassDescription> _subpasses;
    std::vector<vk::SubpassDependency> _subpass_dependencies;

    std::vector<Framebuffer> _framebuffers;

    vk::RenderPass _render_pass;

    void _default_attachments(const Swapchain &swapchain);
    void _default_subpasses();
    void _default_subpass_dependencies();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERPASS_HPP