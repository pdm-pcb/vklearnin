#ifndef VKLEARNIN_RENDERING_PASSES_COLORPASS_HPP
#define VKLEARNIN_RENDERING_PASSES_COLORPASS_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/pipelines/vkRenderPass.hpp"

namespace vkl {

class vkSurface;
class vkDevice;

class ColorPass final {
public:
    ColorPass() = default;
    ~ColorPass() = default;

    ColorPass(ColorPass &&) = delete;
    ColorPass(ColorPass const &) = delete;

    ColorPass & operator=(ColorPass &&) = delete;
    ColorPass & operator=(ColorPass const &) = delete;

    bool create(vkSurface const &surface,
                std::span<vk::ClearValue const> const clear_values,
                vkDevice const &device);
    bool destroy();

    void update_render_area(vkSurface const &surface);

    inline auto & begin_info() { return _begin_info; }

    inline auto const & render_pass() const { return _render_pass; }

private:
    vkRenderPass _render_pass;

    vk::RenderPassBeginInfo _begin_info { };

    std::vector<vk::AttachmentDescription> _attachment_descriptions;

    std::vector<vk::AttachmentReference> _color_refs;

    std::vector<vk::SubpassDescription> _subpass_descriptions;
    std::vector<vk::SubpassDependency> _subpass_deps;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_PASSES_COLORPASS_HPP