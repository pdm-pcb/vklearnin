#ifndef VKLEARNIN_MESH_VERTEX_HPP
#define VKLEARNIN_MESH_VERTEX_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct Vertex {
    glm::vec4 position;
    glm::vec2 texcoord;

    const static std::vector<vk::VertexInputBindingDescription>   bindings;
    const static std::vector<vk::VertexInputAttributeDescription> attribs;
};

} // namespace vkl

#endif // VKLEARNIN_MESH_VERTEX_HPP