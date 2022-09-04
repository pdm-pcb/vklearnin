#ifndef VKLEARNIN_TEXTURES_SAMPLER2D_HPP
#define VKLEARNIN_TEXTURES_SAMPLER2D_HPP

#include "vklearnin/pch.hpp"

class Sampler2D {
public:
    void init(const vk::Filter min_filter,
              const vk::Filter mag_filter,
              const uint32_t mip_levels,
              const vk::SamplerMipmapMode mipmap_mode,
              const vk::SamplerAddressMode address_mode_u,
              const vk::SamplerAddressMode address_mode_v,
              const vk::Bool32 enable_anisotropy, const float max_anisotropy);

    inline vk::Sampler handle() const { return _sampler; }

    explicit Sampler2D(const vk::Device &device);
    ~Sampler2D();

    Sampler2D() = delete;

    Sampler2D(Sampler2D &&other) = delete;
    Sampler2D(const Sampler2D &other) = delete;

    Sampler2D & operator=(Sampler2D &&other) = delete;
    Sampler2D & operator=(const Sampler2D &other) = delete;

private:
    vk::Sampler _sampler;

    const vk::Device &_device;
};

#endif // VKLEARNIN_TEXTURES_SAMPLER2D_HPP