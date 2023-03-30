#ifndef VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP
#define VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"
#include "vklearnin/materials/Texture2D.hpp"

namespace vkl {

class RenderPass final {
public:
    RenderPass & create_color_buffer(vk::Extent2D const &extent);
    RenderPass & create_depth_buffer(vk::Extent2D const &extent);
    RenderPass & default_color_attachments();
    RenderPass & default_color_subpass();

    RenderPass & create_shadow_map(vk::Extent2D const &extent);
    RenderPass & default_shadow_map_attachments();
    RenderPass & default_shadow_map_subpass();

    void create();
    void destroy();

    inline auto const& native() const { return _render_pass; }

    inline auto const& color_buffer() const { return _color_buffer; }
    inline auto const& depth_buffer() const { return _depth_buffer; }

    inline auto const& shadow_map() const { return _shadow_map;}

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

    vk::Format _depth_format;

    vk::RenderPass _render_pass;

    ImageObject _color_buffer;
    ImageObject _depth_buffer;

    Texture2D _shadow_map;
    vk::AttachmentReference _shadow_map_attachment;

    void _find_depth_stencil_format();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP