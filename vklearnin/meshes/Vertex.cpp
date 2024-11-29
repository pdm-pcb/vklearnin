#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Vertex.hpp"

namespace vkl {

Vertex::Bindings const Vertex::bindings {{
    .binding   = 0u,
    .stride    = sizeof(Vertex),
    .inputRate = vk::VertexInputRate::eVertex
}};

Vertex::Attributes const Vertex::attributes {{
    .location = 0u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(Vertex, position)),
},
{
    .location = 2u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(Vertex, uv_coord)),
}};

// =============================================================================

Vertex::Vertex(glm::vec4 const &a_position,
               glm::vec2 const &a_uv_coord) :
    position { a_position },
    uv_coord { a_uv_coord }
{ }

} // namespace vkl