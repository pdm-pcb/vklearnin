#ifndef VKLEARNIN_MESHES_VERTEX_HPP
#define VKLEARNIN_MESHES_VERTEX_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

struct Vertex final {
    glm::vec4 position { };
    glm::vec2 uv_coord { };

    using Bindings   = std::vector<vk::VertexInputBindingDescription>;
    using Attributes = std::vector<vk::VertexInputAttributeDescription>;
    static Bindings   const bindings;
    static Attributes const attributes;

    Vertex(glm::vec4 const &a_position,
           glm::vec2 const &a_uv_coord);
    Vertex() = delete;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_VERTEX_HPP