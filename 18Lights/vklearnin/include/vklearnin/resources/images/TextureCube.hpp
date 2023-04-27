#ifndef VKLEARNIN_RESOURCES_IMAGES_TEXTURECUBE_HPP
#define VKLEARNIN_RESOURCES_IMAGES_TEXTURECUBE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/resources/images/ImageObject.hpp"

namespace vkl {

class TextureCube final {
public:
    using FilePaths = std::array<std::string_view, 6>;
    void create(FilePaths const &filepaths, bool use_mipmaps = true);
    void destroy();

    inline auto & image() { return _image; }
    inline auto const & image() const { return _image; }

    TextureCube();
    ~TextureCube() = default;

    TextureCube(TextureCube &&other) noexcept;
    TextureCube(const TextureCube &) = default;

    TextureCube& operator=(TextureCube &&other) noexcept;
    TextureCube& operator=(const TextureCube &) = default;

private:
    ImageObject _image;
};

} // namespace vkl

#endif // VKLEARNIN_RESOURCES_IMAGES_TEXTURECUBE_HPP