#ifndef VKLEARNIN_TEXTURES_TEXTURE2D_HPP
#define VKLEARNIN_TEXTURES_TEXTURE2D_HPP

#include "vklearnin/Shaders/Buffers/StagingBuffer.hpp"
#include "vklearnin/Textures/Sampler2D.hpp"
#include "vklearnin/Tools/Allocator.hpp"

#include <cstdint>

class Instance;

class Texture2D {
public:
    enum BPC : int {
        R    = 1,
        RG   = 2,
        RGB  = 3,
        RGBA = 4
    };

    void load_file(const char *filepath, const bool flip_vertical = false);
    void init_image_view();
    void init_sampler(const vk::Filter min_filter, const vk::Filter mag_filter,
                      const vk::SamplerMipmapMode mipmap_mode,
                      const vk::SamplerAddressMode address_mode_u,
                      const vk::SamplerAddressMode address_mode_v,
                      const vk::Bool32 enable_anisotropy,
                      const float max_anisotropy);

    inline vk::Sampler     sampler() const { return _sampler.handle(); }
    inline vk::ImageView   view()    const { return _image_view; }
    inline vk::Format      format()  const { return _format; }
    inline vk::ImageLayout layout()  const { return _layout; }

    Texture2D(const vk::CommandPool &pool, const vk::Queue &queue,
              const Instance &instance);
    ~Texture2D();

    Texture2D() = delete;

    Texture2D(Texture2D &&other) = delete;
    Texture2D(const Texture2D &other) = delete;

    Texture2D & operator=(Texture2D &&other) = delete;
    Texture2D & operator=(const Texture2D &other) = delete;

private:
    vk::Image     _image_handle;
    VmaAllocation _device_memory;

    vk::Offset3D _offset;
    vk::Extent3D _extent;

    vk::ImageView   _image_view;
    Sampler2D       _sampler;
    vk::Format      _format;
    vk::ImageLayout _layout;

    uint32_t _mip_levels;

    StagingBuffer<uint8_t> *_staging;
    const vk::CommandPool  &_pool;
    const vk::Queue        &_queue;
    const Instance         &_instance;

    void _create_image();
    void _upload_texture();
    void _generate_mipmaps();
    void _layout_transition(const vk::ImageLayout &old_layout,
                            const vk::ImageLayout &new_layout);
};

#endif // VKLEARNIN_TEXTURES_TEXTURE2D_HPP