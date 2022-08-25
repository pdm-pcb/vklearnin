#ifndef VKLEARNIN_TEXTURES_TEXTURE2D_HPP
#define VKLEARNIN_TEXTURES_TEXTURE2D_HPP

class Texture2D {
public:
    void load_file(const char *filepath);

    Texture2D();
    ~Texture2D();
};

#endif // VKLEARNIN_TEXTURES_TEXTURE2D_HPP