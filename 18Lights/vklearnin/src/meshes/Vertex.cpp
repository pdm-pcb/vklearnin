#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Vertex.hpp"

namespace vkl {

// =============================================================================
Vertex::Bindings const Vertex::bindings {{
    .binding   = 0u,
    .stride    = sizeof(Vertex),
    .inputRate = vk::VertexInputRate::eVertex
}};

Vertex::Attribs const Vertex::attributes {{
    .location = 0u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(Vertex, position)),
},
{
    .location = 1u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(Vertex, color)),
},
{
    .location = 2u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(Vertex, normal)),
},
{
    .location = 3u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(Vertex, uv)),
}};

} // namespace vkl