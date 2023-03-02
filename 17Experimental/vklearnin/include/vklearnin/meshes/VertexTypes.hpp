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

    VertexFlatColor(const std::array<float, 4> &position,
                    const std::array<float, 4> &color);
    VertexFlatColor() = delete;
};

// =============================================================================
struct VertexFlatTexture final {
    std::array<float, 4> position;
    std::array<float, 2> uv;

    static VertexBindings const bindings;
    static VertexAttribs  const attributes;

    VertexFlatTexture(const std::array<float, 4> &position,
                      const std::array<float, 2> &uv);
    VertexFlatTexture() = delete;
};

// =============================================================================
struct VertexSkybox final {
    std::array<float, 4> position;

    static VertexBindings const bindings;
    static VertexAttribs  const attributes;

    VertexSkybox(const std::array<float, 4> &position);
    VertexSkybox() = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_VERTEXTYPES_HPP