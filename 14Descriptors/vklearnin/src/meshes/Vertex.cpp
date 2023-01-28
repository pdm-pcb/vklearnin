#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Vertex.hpp"

namespace vkl {

// The input binding description tells a pipeline that...
const Vertex::BindingDescriptions Vertex::_binding_desc {{
    // These data will be bound to the zeroth slot
    .binding   = 0u,

    // The space between the beginning of one vertex's worth of information
    // and the beginning of the next will be the size of this class, or
    // 2 * 4 * sizeof(float)
    .stride    = sizeof(Vertex),

    // And that the data blocks described by the above stride size are going to
    // be per-vertex in nature, as opposed to per-instance if we were drawing
    // several copies of the same geometry
    .inputRate = vk::VertexInputRate::eVertex
}};

// While the binding description tells a pipeline where to find data that often
// repeat (one copy for each vertex, in our case) the attribute description
// provides clarification for each member of those repeating blocks
const Vertex::AttribDescriptions Vertex::_attrib_desc {
    {
        // Location here corresponds to the location specified at the top of
        // our shader's source code
        .location = 0u,

        // This binding matches the one above
        .binding  = 0u,

        // Because I'm using four floats to describe both position and color,
        // the format specifies 32 bits for each. In the case of vk::Format,
        // the ideas of RGBA and XYZW are interchangable.
        .format   = vk::Format::eR32G32B32A32Sfloat,

        // This usage of the offsetof macro will resolve to zero, but for color
        // below, it'll provide an offset of 16 bytes - the same as four floats.
        .offset   = static_cast<uint32_t>(offsetof(Vertex, _position)),
    },
    {
        .location = 1u,
        .binding  = 0u,
        .format   = vk::Format::eR32G32B32A32Sfloat,
        .offset   = static_cast<uint32_t>(offsetof(Vertex, _color)),
    }
};

// =============================================================================
Vertex::Vertex(const std::array<float, 4> &position,
               const std::array<float, 4> &color) :
    _position { position },
    _color    { color }
{ }

} // namespace vkl