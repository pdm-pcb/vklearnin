#ifndef VKLEARNIN_TEXTURES_SAMPLER2D_HPP
#define VKLEARNIN_TEXTURES_SAMPLER2D_HPP

#include <vulkan/vulkan.h>

class Sampler2D {
public:
    void init(const ::VkFilter min_filter, const ::VkFilter mag_filter,
              const ::VkSamplerMipmapMode mipmap_mode,
              const ::VkSamplerAddressMode address_mode_u,
              const ::VkSamplerAddressMode address_mode_v,
              const ::VkBool32 enable_anisotropy, const float max_anisotropy);

    inline ::VkSampler handle() const { return _sampler; }

    Sampler2D(const ::VkDevice &device);
    ~Sampler2D();

    Sampler2D() = delete;

private:
    ::VkSampler _sampler;

    const ::VkDevice &_device;
};

#endif // VKLEARNIN_TEXTURES_SAMPLER2D_HPP