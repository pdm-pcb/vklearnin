#ifndef VKLEARNIN_SHADERS_TEXTURE2D_HPP
#define VKLEARNIN_SHADERS_TEXTURE2D_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/shaders/ImageObject.hpp"

namespace vkl {

class Texture2D {
public:
    void load_from_file(const char *filepath, const bool flip_vertical = false);
    void destroy();

    const auto & image() const { return _image; }

    Texture2D();
    ~Texture2D() = default;

    Texture2D(Texture2D &&other) = delete;
    Texture2D(const Texture2D &other) = delete;

    Texture2D & operator=(Texture2D &&other) = delete;
    Texture2D & operator=(const Texture2D &other) = delete;

private:
    uint32_t _width;
    uint32_t _height;
    uint32_t _channels;
    size_t   _size_bytes;

    ImageObject _image;
};

} // namespace vkl

#endif // VKLEARNIN_SHADERS_TEXTURE2D_HPP