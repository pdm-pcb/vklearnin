#include "vklearnin/vklearnin.hpp"
#include "vklearnin/mesh/Vertex.hpp"

namespace vkl {

const std::vector<vk::VertexInputBindingDescription> Vertex::bindings {
    {
        .binding = 0u,
        .stride = sizeof(Vertex),
        .inputRate = vk::VertexInputRate::eVertex
    }
};

const std::vector<vk::VertexInputAttributeDescription> Vertex::attribs {
    {
        .location = 0u,
        .binding = 0u,
        .format = vk::Format::eR32G32B32A32Sfloat,
        .offset = offsetof(Vertex, position)
    },
    {
        .location = 1u,
        .binding = 0u,
        .format = vk::Format::eR32G32B32A32Sfloat,
        .offset = offsetof(Vertex, normal)
    },
    {
        .location = 2u,
        .binding = 0u,
        .format = vk::Format::eR32G32B32A32Sfloat,
        .offset = offsetof(Vertex, tangent)
    },
    {
        .location = 3u,
        .binding = 0u,
        .format = vk::Format::eR32G32B32A32Sfloat,
        .offset = offsetof(Vertex, bitangent)
    },
    {
        .location = 4u,
        .binding = 0u,
        .format = vk::Format::eR32G32Sfloat,
        .offset = offsetof(Vertex, texcoord)
    }
};

} // namespace vkl