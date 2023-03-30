#ifndef VKLEARNIN_MATERIALS_TEXTURE2D_HPP
#define VKLEARNIN_MATERIALS_TEXTURE2D_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

struct BufferObject;

class Texture2D final {
public:
    using Filepath = std::string_view;
    void texture_from_file(Filepath filepath);

    using CubeFilepaths = std::array<std::string_view, 6>;
    void cubemap_from_files(CubeFilepaths const &filepaths);

    void create_shadow_map(vk::Extent2D const &extent,
                           vk::Format const depth_format);

    void create_sampler(vk::Filter const min_filter,
                        vk::Filter const mag_filter,
                        vk::SamplerMipmapMode const mip_filter,
                        vk::SamplerAddressMode const mode_u,
                        vk::SamplerAddressMode const mode_v,
                        vk::Bool32 const enable_compare,
                        vk::CompareOp const compare_op);

    void generate_mipmap(vk::Filter const mip_filter);

    void destroy();

    inline auto const& image() const { return _image; }

    Texture2D();
    ~Texture2D() = default;

    Texture2D(Texture2D &&other) noexcept;
    Texture2D(const Texture2D &) = default;

    Texture2D& operator=(Texture2D &&other) noexcept;
    Texture2D& operator=(const Texture2D &) = default;

private:
    ImageObject _image;

    void _calc_mip_levels();
};

} // namespace vkl

#endif // VKLEARNIN_MATERIALS_TEXTURE2D_HPP