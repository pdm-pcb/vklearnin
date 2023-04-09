#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/targets/ShadowMap.hpp"

namespace vkl {

// =============================================================================
void ShadowMap::create(vk::Rect2D const &render_area, vk::Format const format) {
    if(render_area.extent.height == 0u || render_area.extent.width == 0u) {
        CONSOLE_CRITICAL("Cannot create color buffer of zero size.");
        return;
    }

    if(_texture.image().handle) {
        CONSOLE_CRITICAL("Destroy existing color buffer.");
        return;
    }

    _render_area = render_area;

    _texture.create_shadow_map(_render_area.extent, format);

    _texture.create_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eClampToBorder,
        vk::SamplerAddressMode::eClampToBorder,
        VK_TRUE,
        vk::CompareOp::eLessOrEqual
    );
}

// =============================================================================
void ShadowMap::destroy() {
    _texture.destroy();
}

// =============================================================================
ShadowMap::ShadowMap() :
    _render_area { },
    _texture { }
{ }

ShadowMap::ShadowMap(ShadowMap &&other) noexcept :
    _render_area { std::move(other._render_area) },
    _texture { std::move(other._texture) }
{
    other._render_area = vk::Rect2D { };
    other._texture = Texture2D { };
}

} // namespace vkl