#ifndef VKLEARNIN_RENDERPASSES_COLORPASS_HPP
#define VKLEARNIN_RENDERPASSES_COLORPASS_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/pipelines/vkRenderPass.hpp"

namespace vkl {

class vkSurface;
class vkDevice;
class vkFrameBuffer;
class vkCmdBuffer;

class ColorPass final {
public:
    ColorPass() = default;
    ~ColorPass() = default;

    ColorPass(ColorPass &&) = delete;
    ColorPass(ColorPass const &) = delete;

    ColorPass & operator=(ColorPass &&) = delete;
    ColorPass & operator=(ColorPass const &) = delete;

    bool create(vkSurface const &surface, vkDevice const &device);
    bool destroy();

    void update_render_area(vkSurface const &surface);

    void begin(vkFrameBuffer const &frame_buffer,
               std::span<vk::ClearValue const> const clear_values,
               vkCmdBuffer const &cmd_buffer);

    inline auto const & render_pass() const { return _render_pass; }

private:
    vkRenderPass _render_pass;

    std::vector<vk::AttachmentDescription> _attachment_descriptions;
    std::vector<vk::AttachmentReference>   _color_refs;
    std::vector<vk::SubpassDescription>    _subpass_descs;
    std::vector<vk::SubpassDependency>     _subpass_deps;

    vk::Rect2D _render_area { };
};

} // namespace vkl

#endif // VKLEARNIN_RENDERPASSES_COLORPASS_HPP