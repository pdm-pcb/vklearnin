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

VertexFlatColor::VertexFlatColor(Vec4 const &position, Vec4 const &color) :
    position { position },
    color    { color }
{ }

// =============================================================================
VertexBindings const VertexLitColor::bindings {{
    .binding   = 0u,
    .stride    = sizeof(VertexLitColor),
    .inputRate = vk::VertexInputRate::eVertex
}};

VertexAttribs const VertexLitColor::attributes {{
    .location = 0u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexLitColor, position)),
},
{
    .location = 1u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexLitColor, normal)),
},
{
    .location = 2u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexLitColor, color)),
}};

VertexLitColor::VertexLitColor(Vec4 const &position, Vec4 const &normal,
                               Vec4 const &color) :
    position { position },
    normal   { normal },
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

VertexFlatTexture::VertexFlatTexture(Vec4 const &position,
                                     std::array<float, 2> const &uv) :
    position { position },
    uv       { uv }
{ }

// =============================================================================
VertexBindings const VertexSkybox::bindings {{
    .binding   = 0u,
    .stride    = sizeof(VertexSkybox),
    .inputRate = vk::VertexInputRate::eVertex
}};

VertexAttribs const VertexSkybox::attributes {{
    .location = 0u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32A32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(VertexSkybox, position)),
}};

VertexSkybox::VertexSkybox(Vec4 const &position) :
    position { position }
{ }

} // namespace vkl