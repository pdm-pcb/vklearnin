#ifndef VKLEARNIN_TEXTURES_TEXTURE2D_HPP
#define VKLEARNIN_TEXTURES_TEXTURE2D_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/resources/vkImage.hpp"
#include "vklearnin/vulkan/resources/vkImageView.hpp"
#include "vklearnin/vulkan/resources/vkSampler.hpp"

namespace vkl {

class vkDescriptorPool;
class vkPhsyicalDevice;
class vkDevice;

class Texture2D final {
public:
    Texture2D() = default;
    ~Texture2D() = default;

    Texture2D(Texture2D &&) = delete;
    Texture2D(Texture2D const &) = delete;

    Texture2D & operator=(Texture2D &&) = delete;
    Texture2D & operator=(Texture2D const &) = delete;

    bool create(std::string_view const file_name,
                vkImage::Details const &image_details,
                vk::ImageViewType const view_type,
                vkSampler::Filters const &sampler_filters,
                vkSampler::AddressMode const &sampler_address_mode,
                float const sampler_max_aniso,
                vkDevice const &device);

    bool destroy();

    inline auto const & image()   const { return _image; }
    inline auto const & view()    const { return _view; }
    inline auto const & sampler() const { return _sampler; }

private:
    vkImage     _image;
    vkImageView _view;
    vkSampler   _sampler;
};

} // namespace vkl

#endif // VKLEARNIN_TEXTURES_TEXTURE2D_HPP