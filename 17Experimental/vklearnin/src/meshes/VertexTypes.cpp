#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/VertexTypes.hpp"

namespace vkl {

// =============================================================================
VertexBindings const VertexFlatColor::bindings {{
    .binding   = 0u,
    .stride    = sizeof(VertexFlatColor),
    .inputRate = vk::VertexInputRate::eVertex
}};

VertexAttribs const VertexFlatColor::attributes {{
    .location = 0u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexFlatColor, position)),
},
{
    .location = 1u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexFlatColor, color)),
}};

VertexFlatColor::VertexFlatColor(const std::array<float, 4> &position,
                         const std::array<float, 4> &color) :
    position { position },
    color    { color }
{ }

// =============================================================================
VertexBindings const VertexFlatTexture::bindings {{
    .binding   = 0u,
    .stride    = sizeof(VertexFlatTexture),
    .inputRate = vk::VertexInputRate::eVertex
}};

VertexAttribs const VertexFlatTexture::attributes {{
    .location = 0u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexFlatTexture, position)),
},
{
    .location = 1u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexFlatTexture, uv)),
}};

VertexFlatTexture::VertexFlatTexture(const std::array<float, 4> &position,
                             const std::array<float, 2> &uv) :
    position { position },
    uv       { uv }
{ }

// =============================================================================
VertexBindings const VertexSkybox::bindings {{
    .binding   = 0u,
    .stride    = sizeof(VertexFlatTexture),
    .inputRate = vk::VertexInputRate::eVertex
}};

VertexAttribs const VertexSkybox::attributes {{
    .location = 0u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexSkybox, position)),
}};

VertexSkybox::VertexSkybox(const std::array<float, 4> &position) :
    position { position }
{ }

} // namespace vkl