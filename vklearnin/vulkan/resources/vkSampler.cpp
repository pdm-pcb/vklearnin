#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/resources/vkSampler.hpp"

#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
bool vkSampler::create(Filters const filters,
                       AddressMode const address_mode,
                       float const max_aniso,
                       vkDevice const &device)
{
    if(_handle) {
        Log::error("Sampler {} already exists", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create sampler with invalid device.");
        return false;
    }

    _device = device.native();

    vk::SamplerCreateInfo const create_info {
        .magFilter        = filters.mag,
        .minFilter        = filters.min,
        .mipmapMode       = filters.mip,
        .addressModeU     = address_mode.u,
        .addressModeV     = address_mode.v,
        .mipLodBias       = 0.0f,
        .anisotropyEnable = vk::True,
        .maxAnisotropy    = max_aniso,
        .compareEnable    = vk::False,
        .compareOp        = vk::CompareOp::eAlways,
        .minLod           = 0.0f,
        .maxLod           = VK_LOD_CLAMP_NONE,
        .borderColor      = vk::BorderColor::eIntOpaqueWhite,
        .unnormalizedCoordinates = vk::False
    };

    _handle = _device.createSampler(create_info);
    Log::trace("\nCreated image sampler {}"
               "\n    Mag Filter: {}"
               "\n    Min Filter: {}"
               "\n    Mip Mode:   {}"
               "\n    Address U:  {}"
               "\n    Address V:  {}"
               "\n    Anisotropy: {}",
               _handle,
               vk::to_string(create_info.magFilter),
               vk::to_string(create_info.minFilter),
               vk::to_string(create_info.mipmapMode),
               vk::to_string(create_info.addressModeU),
               vk::to_string(create_info.addressModeV),
               create_info.maxAnisotropy);

    return true;
}

// =============================================================================
bool vkSampler::destroy() {
    if(!_handle) {
        Log::error("Must create sampler before calling destroy.");
        return false;
    }

    _device.destroySampler(_handle);

    _handle = nullptr;
    _device = nullptr;

    return true;
}

} // namespace vkl