#ifndef VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP
#define VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

class RenderPass final {
public:
    void create();
    void destroy();

    inline auto const& native() const { return _render_pass; }
    inline auto const color_buffer_view() const { return _color_buffer.view; }
    inline auto const depth_buffer_view() const { return _depth_buffer.view; }

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
    std::vector<vk::SubpassDescription>    _subpasses;
    std::vector<vk::SubpassDependency>     _subpass_deps;

    vk::RenderPass _render_pass;

    vk::SampleCountFlagBits _samples;

    ImageObject _color_buffer;
    ImageObject _depth_buffer;

    void _init_color_buffer();
    void _get_sample_count();
    void _init_depth_buffer();
    void _default_attachments();
    void _default_subpasses();

    void _find_depth_stencil_format();
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGRENDERPASS_RENDERPASS_HPP