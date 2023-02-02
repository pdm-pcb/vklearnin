#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/VertexTypes.hpp"

namespace vkl {

// =============================================================================
VertexBindings const VertexColor::bindings {{
    .binding   = 0u,
    .stride    = sizeof(VertexColor),
    .inputRate = vk::VertexInputRate::eVertex
}};

VertexAttribs const VertexColor::attributes {{
    .location = 0u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexColor, position)),
},
{
    .location = 1u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexColor, color)),
}};

VertexColor::VertexColor(const std::array<float, 4> &position,
                         const std::array<float, 4> &color) :
    position { position },
    color    { color }
{ }

// =============================================================================
VertexBindings const VertexTexture::bindings {{
    .binding   = 0u,
    .stride    = sizeof(VertexTexture),
    .inputRate = vk::VertexInputRate::eVertex
}};

VertexAttribs const VertexTexture::attributes {{
    .location = 0u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexTexture, position)),
},
{
    .location = 1u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexTexture, uv)),
}};

VertexTexture::VertexTexture(const std::array<float, 4> &position,
                             const std::array<float, 2> &uv) :
    position { position },
    uv       { uv }
{ }

} // namespace vkl