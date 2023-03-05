#ifndef VKLEARNIN_MESHES_VERTEXTYPES_HPP
#define VKLEARNIN_MESHES_VERTEXTYPES_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

using VertexBindings = std::vector<vk::VertexInputBindingDescription>;
using VertexAttribs  = std::vector<vk::VertexInputAttributeDescription>;

// =============================================================================
struct VertexFlatColor final {
    Vec4 position;
    Vec4 color;

    static VertexBindings const bindings;
    static VertexAttribs  const attributes;

    VertexFlatColor(Vec4 const &position, Vec4 const &color);
    VertexFlatColor() = delete;
};

// =============================================================================
struct VertexLitColor final {
    Vec4 position;
    Vec4 normal;
    Vec4 color;

    static VertexBindings const bindings;
    static VertexAttribs  const attributes;

    VertexLitColor(Vec4 const &position, Vec4 const &normal, Vec4 const &color);
    VertexLitColor() = delete;
};

// =============================================================================
struct VertexFlatTexture final {
    Vec4 position;
    std::array<float, 2> uv;

    static VertexBindings const bindings;
    static VertexAttribs  const attributes;

    VertexFlatTexture(Vec4 const &position, std::array<float, 2> const &uv);
    VertexFlatTexture() = delete;
};

// =============================================================================
struct VertexSkybox final {
    Vec4 position;

    static VertexBindings const bindings;
    static VertexAttribs  const attributes;

    VertexSkybox(Vec4 const &position);
    VertexSkybox() = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_VERTEXTYPES_HPP