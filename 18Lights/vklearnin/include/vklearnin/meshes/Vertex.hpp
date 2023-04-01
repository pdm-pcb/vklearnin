#ifndef VKLEARNIN_MESHES_VERTEX_HPP
#define VKLEARNIN_MESHES_VERTEX_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

// =============================================================================
struct Vertex final {
    Vec4 position { Vec4::origin };
    Vec4 color    { color::white, 1.0f };
    Vec4 normal   { Vec4::origin };
    Vec2 uv       { 0.0f, 0.0f };

    using Bindings = std::vector<vk::VertexInputBindingDescription>;
    static Bindings const bindings;

    using Attribs  = std::vector<vk::VertexInputAttributeDescription>;
    static Attribs  const attributes;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_VERTEX_HPP