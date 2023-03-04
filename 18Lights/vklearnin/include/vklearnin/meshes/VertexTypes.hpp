#ifndef VKLEARNIN_MESHES_VERTEXTYPES_HPP
#define VKLEARNIN_MESHES_VERTEXTYPES_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

using VertexBindings = std::vector<vk::VertexInputBindingDescription>;
using VertexAttribs  = std::vector<vk::VertexInputAttributeDescription>;

// =============================================================================
struct VertexFlatColor final {
    std::array<float, 4> position;
    std::array<float, 4> color;

    static VertexBindings const bindings;
    static VertexAttribs  const attributes;

    VertexFlatColor(std::array<float, 4> const &position,
                    std::array<float, 4> const &color);
    VertexFlatColor() = delete;
};

// =============================================================================
struct VertexLitColor final {
    std::array<float, 4> position;
    Vec4 normal;
    std::array<float, 4> color;

    static VertexBindings const bindings;
    static VertexAttribs  const attributes;

    VertexLitColor(std::array<float, 4> const &position,
                   Vec4 const &normal,
                   std::array<float, 4> const &color);
    VertexLitColor() = delete;
};

// =============================================================================
struct VertexFlatTexture final {
    std::array<float, 4> position;
    std::array<float, 2> uv;

    static VertexBindings const bindings;
    static VertexAttribs  const attributes;

    VertexFlatTexture(std::array<float, 4> const &position,
                      std::array<float, 2> const &uv);
    VertexFlatTexture() = delete;
};

// =============================================================================
struct VertexSkybox final {
    std::array<float, 4> position;

    static VertexBindings const bindings;
    static VertexAttribs  const attributes;

    VertexSkybox(std::array<float, 4> const &position);
    VertexSkybox() = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_VERTEXTYPES_HPP