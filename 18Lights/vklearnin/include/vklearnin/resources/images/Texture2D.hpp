#ifndef VKLEARNIN_RESOURCES_IMAGES_TEXTURE2D_HPP
#define VKLEARNIN_RESOURCES_IMAGES_TEXTURE2D_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

class Texture2D final {
public:
    using FilePath = std::string_view;
    void create(FilePath filepath, bool use_mipmaps = true);
    void destroy();

    inline auto & image() { return _image; }
    inline auto const & image() const { return _image; }

    Texture2D();
    ~Texture2D() = default;

    Texture2D(Texture2D &&other) noexcept;
    Texture2D(const Texture2D &) = default;

    Texture2D& operator=(Texture2D &&other) noexcept;
    Texture2D& operator=(const Texture2D &) = default;

private:
    ImageObject _image;
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_IMAGES_TEXTURE2D_HPP