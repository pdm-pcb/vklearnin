#ifndef VKLEARNIN_TEXTURES_TEXTURE2D_HPP
#define VKLEARNIN_TEXTURES_TEXTURE2D_HPP

#include "vklearnin/Buffers/StagingBuffer.hpp"

#include <cstdint>

class Instance;

class Texture2D {
public:
    void load_file(const char *filepath);

    Texture2D(const ::VkCommandPool &pool, const ::VkQueue &queue,
              const Instance &instance);
    ~Texture2D();

private:
    ::VkImage        _image_handle;
    ::VkFormat       _format;
    ::VkDeviceMemory _device_memory;

    ::VkOffset3D _offset;
    ::VkExtent3D _extent;

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