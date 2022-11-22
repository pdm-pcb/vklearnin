#ifndef VKLEARNIN_MESH_VERTEX_HPP
#define VKLEARNIN_MESH_VERTEX_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct alignas(128) Vertex {
    std::array<float, 4> position;
    std::array<float, 4> color;
    std::array<float, 2> texcoord;

    const static std::vector<vk::VertexInputBindingDescription>   bindings;
    const static std::vector<vk::VertexInputAttributeDescription> attribs;
};

} // namespace vkl

#endif // VKLEARNIN_MESH_VERTEX_HPP