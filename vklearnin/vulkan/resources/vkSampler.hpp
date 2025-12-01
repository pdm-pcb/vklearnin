#ifndef VKLEARNIN_VULKAN_RESOURCES_VKSAMPLER_HPP
#define VKLEARNIN_VULKAN_RESOURCES_VKSAMPLER_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkPhysicalDevice;
class vkDevice;

class vkSampler final {
public:
    vkSampler() = default;
    ~vkSampler() = default;

    vkSampler(vkSampler &&) = delete;
    vkSampler(vkSampler const &) = delete;

    vkSampler & operator=(vkSampler &&) = delete;
    vkSampler & operator=(vkSampler const &) = delete;

    struct Filters final {
        vk::Filter mag { vk::Filter::eLinear };
        vk::Filter min { vk::Filter::eLinear };
        vk::SamplerMipmapMode mip { vk::SamplerMipmapMode::eLinear };
    };

    struct AddressMode final {
        vk::SamplerAddressMode u { vk::SamplerAddressMode::eClampToEdge };
        vk::SamplerAddressMode v { vk::SamplerAddressMode::eClampToEdge };
    };

    bool create(Filters const filters,
                AddressMode const address_mode,
                float const max_aniso,
                vkDevice const &device);
    bool destroy();

    auto const & native() const { return _handle; }

private:
    vk::Sampler _handle { nullptr };
    vk::Device  _device { nullptr };
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_RESOURCES_VKSAMPLER_HPP