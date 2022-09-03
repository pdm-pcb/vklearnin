#include "vklearnin/common.hpp"
#include "vklearnin/Textures/Sampler2D.hpp"

void Sampler2D::init(const vk::Filter min_filter,
                     const vk::Filter mag_filter,
                     const uint32_t mip_levels,
                     const vk::SamplerMipmapMode mipmap_mode,
                     const vk::SamplerAddressMode address_mode_u,
                     const vk::SamplerAddressMode address_mode_v,
                     const vk::Bool32 enable_anisotropy,
                     const float max_anisotropy)
{
    CONSOLE_INFO("");

    vk::SamplerCreateInfo sampler_info {
        .magFilter = min_filter,
        .minFilter = mag_filter,
        .mipmapMode = mipmap_mode,
        .addressModeU = address_mode_u,
        .addressModeV = address_mode_v,
        // repeat is the default (0u), but W shouldn't matter for 2D samplers
        .addressModeW = vk::SamplerAddressMode::eRepeat,
        .mipLodBias = 0.0f,
        .anisotropyEnable = enable_anisotropy,
        .maxAnisotropy = max_anisotropy,
        .compareEnable = false,
        .compareOp = vk::CompareOp::eNever,
        .minLod = 0.0f,
        .maxLod = static_cast<float>(mip_levels),
        .borderColor = vk::BorderColor::eIntOpaqueBlack,
        .unnormalizedCoordinates = false,
    };

    _sampler = _device.createSampler(sampler_info);
}

Sampler2D::Sampler2D(const vk::Device &device) :
    _sampler { nullptr },
    _device  { device  }
{
    CONSOLE_INFO("");
}

Sampler2D::~Sampler2D() {
    CONSOLE_INFO("");

    _device.destroy(_sampler);
}