#include "vklearnin/vklearnin.hpp"
#include "vklearnin/mesh/Vertex.hpp"

namespace vkl {

const std::vector<vk::VertexInputBindingDescription> Vertex::bindings {{
    .binding = 0u,
    .stride = sizeof(Vertex),
    .inputRate = vk::VertexInputRate::eVertex
}};

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
    .offset = offsetof(Vertex, color)
}};

} // namespace vkl