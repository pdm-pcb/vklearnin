#ifndef VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP
#define VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class RenderPass final {
public:
    void create();
    void destroy();

    inline auto native() const { return _render_pass; }

    RenderPass();
    ~RenderPass() = default;

    RenderPass(RenderPass &&) = delete;
    RenderPass(const RenderPass &) = delete;

    RenderPass& operator=(RenderPass &&) = delete;
    RenderPass& operator=(const RenderPass &) = delete;

private:
    std::vector<vk::AttachmentDescription> _attach_descs;
    std::vector<vk::AttachmentReference>   _color_attachments;
    std::vector<vk::SubpassDescription>    _subpasses;
    std::vector<vk::SubpassDependency>     _subpass_deps;

    vk::RenderPass _render_pass;

    void _default_attachments();
    void _default_subpasses();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP