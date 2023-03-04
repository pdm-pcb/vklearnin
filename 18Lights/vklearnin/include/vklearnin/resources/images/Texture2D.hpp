#ifndef VKLEARNIN_RESOURCES_IMAGES_TEXTURE2D_HPP
#define VKLEARNIN_RESOURCES_IMAGES_TEXTURE2D_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

struct BufferObject;

class Texture2D final {
public:
    void texture_from_file(std::string_view filepath);
    void cubemap_from_files(std::array<std::string_view, 6> filepaths);

    void init_sampler(const vk::Filter min_filter,
                      const vk::Filter mag_filter,
                      const vk::SamplerMipmapMode mip_filter,
                      const vk::SamplerAddressMode mode_u,
                      const vk::SamplerAddressMode mode_v);
    void shutdown();

    inline auto const& image() const { return _image; }

    Texture2D();
    ~Texture2D() = default;

    Texture2D(Texture2D &&) = delete;
    Texture2D(const Texture2D &) = default;

    Texture2D& operator=(Texture2D &&) = delete;
    Texture2D& operator=(const Texture2D &) = default;

private:
    ImageObject  _image;

    void _calc_mip_levels();
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_IMAGES_TEXTURE2D_HPP