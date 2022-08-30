#ifndef VKLEARNIN_TEXTURES_TEXTURE2D_HPP
#define VKLEARNIN_TEXTURES_TEXTURE2D_HPP

#include "vklearnin/Shaders/Buffers/StagingBuffer.hpp"
#include "vklearnin/Textures/Sampler2D.hpp"

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
    void init_sampler(const ::VkFilter min_filter, const ::VkFilter mag_filter,
                      const ::VkSamplerMipmapMode mipmap_mode,
                      const ::VkSamplerAddressMode address_mode_u,
                      const ::VkSamplerAddressMode address_mode_v,
                      const ::VkBool32 enable_anisotropy,
                      const float max_anisotropy);

    inline ::VkSampler     sampler() const { return _sampler.handle(); }
    inline ::VkImageView   view()    const { return _view;   }
    inline ::VkFormat      format()  const { return _format; }
    inline ::VkImageLayout layout()  const { return _layout; }

    Texture2D(const ::VkCommandPool &pool, const ::VkQueue &queue,
              const Instance &instance);
    ~Texture2D();

private:
    ::VkImage        _image_handle;
    ::VkDeviceMemory _device_memory;

    ::VkOffset3D _offset;
    ::VkExtent3D _extent;

    ::VkImageView   _view;
    Sampler2D       _sampler;
    ::VkFormat      _format;
    ::VkImageLayout _layout;

    StagingBuffer<uint8_t> *_staging;
    const ::VkCommandPool  &_pool;
    const ::VkQueue        &_queue;
    const Instance         &_instance;

    void _create_image();
    void _upload_texture();
    void _layout_transition(const ::VkImageLayout &old_layout,
                            const ::VkImageLayout &new_layout);
};

#endif // VKLEARNIN_TEXTURES_TEXTURE2D_HPP