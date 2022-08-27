#include "vklearnin/common.hpp"
#include "vklearnin/Textures/Sampler2D.hpp"

void Sampler2D::init(const ::VkFilter min_filter, const ::VkFilter mag_filter,
                     const ::VkSamplerMipmapMode mipmap_mode,
                     const ::VkSamplerAddressMode address_mode_u,
                     const ::VkSamplerAddressMode address_mode_v,
                     const ::VkBool32 enable_anisotropy,
                     const float max_anisotropy)
{
    CONSOLE_INFO("");

    VkSamplerCreateInfo sampler_info {
        .sType = ::VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0u,
        .magFilter = min_filter,
        .minFilter = mag_filter,
        .mipmapMode = mipmap_mode,
        .addressModeU = address_mode_u,
        .addressModeV = address_mode_v,
        // repeat is the default (0u), but W shouldn't matter for 2D samplers
        .addressModeW = ::VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0.0f,
        .anisotropyEnable = enable_anisotropy,
        .maxAnisotropy = max_anisotropy,
        .compareEnable = false,
        .compareOp = ::VK_COMPARE_OP_NEVER,
        .minLod = 0.0f,
        .maxLod = 0.0f,
        .borderColor = ::VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = false,
    };

    auto result = ::vkCreateSampler(
        _device,
        &sampler_info,
        nullptr,
        &_sampler
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Unable to create texture sampler");
    }
}

Sampler2D::Sampler2D(const ::VkDevice &device) :
    _sampler { nullptr },
    _device  { device  }
{
    CONSOLE_INFO("");
}

Sampler2D::~Sampler2D() {
    CONSOLE_INFO("");

    ::vkDestroySampler(_device, _sampler, nullptr);
}