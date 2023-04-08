#ifndef VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP
#define VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"
#include "vklearnin/materials/Texture2D.hpp"

namespace vkl {

class RenderPass final {
public:
    RenderPass & default_color_attachments();
    RenderPass & default_color_subpass();

    RenderPass & depth_color_attachments();
    RenderPass & depth_color_subpass();

    RenderPass & msaa_depth_color_attachments(
        vk::SampleCountFlagBits const &sample_flags
    );
    RenderPass & msaa_depth_color_subpass();

    RenderPass & default_shadow_map_attachments();
    RenderPass & default_shadow_map_subpass();

    void create();
    void destroy();

    inline auto const & native() const { return _render_pass;  }
    inline auto msaa_samples()   const { return _sample_flags; }

    RenderPass();
    ~RenderPass() = default;

    RenderPass(RenderPass &&) = delete;
    RenderPass(const RenderPass &) = delete;

    RenderPass& operator=(RenderPass &&) = delete;
    RenderPass& operator=(const RenderPass &) = delete;

private:
    std::vector<vk::AttachmentDescription> _attach_descs;
    std::vector<vk::AttachmentReference>   _color_attachments;
    vk::AttachmentReference                _depth_attachment;
    std::vector<vk::AttachmentReference>   _resolve_attachments;
    std::vector<vk::SubpassDependency>     _subpass_deps;
    std::vector<vk::SubpassDescription>    _subpasses;

    vk::SampleCountFlagBits _sample_flags;

    vk::RenderPass _render_pass;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP