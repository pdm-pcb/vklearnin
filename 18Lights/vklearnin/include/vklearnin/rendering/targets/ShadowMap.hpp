#ifndef VKLEARNIN_RENDERING_TARGETS_SHADOWMAP_HPP
#define VKLEARNIN_RENDERING_TARGETS_SHADOWMAP_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/Texture2D.hpp"

namespace vkl {

class ShadowMap final {
public:
    void create(vk::Rect2D const &render_area, vk::Format const format);

    void destroy();

    inline auto const & render_area() const { return _render_area;  }

    ShadowMap();
    ~ShadowMap() = default;

    ShadowMap(ShadowMap &&other) noexcept;
    ShadowMap(const ShadowMap &) = delete;

    ShadowMap & operator=(ShadowMap &&) = delete;
    ShadowMap & operator=(const ShadowMap &) = delete;

private:
    vk::Rect2D _render_area;
    Texture2D  _texture;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_TARGETS_COLORBUFFER_HPP