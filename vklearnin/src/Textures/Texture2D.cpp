#include "vklearnin/common.hpp"
#include "vklearnin/Textures/Texture2D.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/CommandStructures/SingleUseCommandBuffer.hpp"

#include "stb/stb_image.h"

// =============================================================================
void Texture2D::load_file(const char *filepath) {
    CONSOLE_INFO("");

    int width;
    int height;
    int channels;

    uint8_t *image_data = ::stbi_load(
        filepath,
        &width,
        &height,
        &channels,
        0
    );

    if(image_data == nullptr) {
		CONSOLE_ERROR("Failed to load image '{}'\n\t"
                      "Size/Channels: {}x{}@{}\n\t"
                      "Error: '{}'",
                      filepath, width, height, channels,
                      ::stbi_failure_reason());
        return;
    }

    auto image_size = static_cast<size_t>(width * height * channels);

    _staging = new StagingBuffer<uint8_t>(
        image_data,
        image_data + image_size,
        _instance
    );

    ::stbi_image_free(image_data);

    _extent.width  = static_cast<uint32_t>(width);
    _extent.height = static_cast<uint32_t>(height);

    switch(channels) {
        case 1: _format = ::VK_FORMAT_R8_SRGB;       break;
        case 2: _format = ::VK_FORMAT_R8G8_SRGB;     break;
        case 3: _format = ::VK_FORMAT_R8G8B8_SRGB;   break;
        case 4: _format = ::VK_FORMAT_R8G8B8A8_SRGB; break;
        default:
            CONSOLE_ERROR(
                "Unsupported image channel count {} for image '{}'",
                channels, filepath
            );
            break;
    }

    _create_image();
    _upload_texture();
}

// =============================================================================
void Texture2D::init_image_view() {
    CONSOLE_INFO("");

    _view = ImageTools::init_view(
        _image_handle,
        _format,
        ::VK_IMAGE_ASPECT_COLOR_BIT,
        _instance.logical_device()
    );
}

// =============================================================================
void Texture2D::init_sampler(const ::VkFilter min_filter,
                             const ::VkFilter mag_filter,
                             const ::VkSamplerMipmapMode mipmap_mode,
                             const ::VkSamplerAddressMode address_mode_u,
                             const ::VkSamplerAddressMode address_mode_v,
                             const ::VkBool32 enable_anisotropy,
                             const float max_anisotropy)
{
    CONSOLE_INFO("");

    _sampler.init(min_filter, mag_filter, mipmap_mode, address_mode_u,
                  address_mode_v, enable_anisotropy, max_anisotropy);
}

// =============================================================================
void Texture2D::_create_image() {
    CONSOLE_INFO("");

    ImageTools::init_image(
        _extent, _format,
        ::VK_IMAGE_TILING_OPTIMAL,
        ::VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        :: VK_IMAGE_USAGE_SAMPLED_BIT,
        ::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        _image_handle, _device_memory,
        _instance
    );
}

// =============================================================================
void Texture2D::_upload_texture() {
    CONSOLE_INFO("");

    _layout_transition(::VK_IMAGE_LAYOUT_UNDEFINED,
                       ::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        SingleUseCommandBuffer command_buffer(_pool, _instance);
        auto command_buffer_handle = command_buffer.init();
        command_buffer.begin();

            ::VkBufferImageCopy copy_region {
                .bufferOffset      = 0u,
                .bufferRowLength   = 0u,
                .bufferImageHeight = 0u,
                .imageSubresource {
                    .aspectMask     = ::VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel       = 0u,
                    .baseArrayLayer = 0u,
                    .layerCount     = 1u
                },
                .imageOffset = _offset,
                .imageExtent = _extent
            };

            ::VkBufferImageCopy copy_regions[] {
                { copy_region }
            };

            ::vkCmdCopyBufferToImage(
                command_buffer_handle,
                _staging->handle(),
                _image_handle,
                ::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                std::size(copy_regions),
                copy_regions
            );

        command_buffer.end();
        command_buffer.submit(_queue);

    _layout_transition(::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       ::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    delete _staging;
}

// =============================================================================
void Texture2D::_layout_transition(const ::VkImageLayout &old_layout,
                                   const ::VkImageLayout &new_layout)
{
    CONSOLE_INFO("");

    SingleUseCommandBuffer command_buffer(_pool, _instance);
    auto command_buffer_handle = command_buffer.init();
    command_buffer.begin();

        ImageTools::layout_transition(
            command_buffer_handle,
            _image_handle,
            _format,
            old_layout,
            new_layout
        );

    command_buffer.end();
    command_buffer.submit(_queue);

    _layout = new_layout;
}

// =============================================================================
Texture2D::Texture2D(const ::VkCommandPool &pool, const ::VkQueue &queue,
                     const Instance &instance) :
    _image_handle { 0u },
    _offset       { 0, 0, 0 },
    _extent       { 0u, 0u, 1u },
    _view         { nullptr },
    _sampler      { Sampler2D(instance.logical_device()) },
    _format       { ::VK_FORMAT_UNDEFINED },
    _layout       { ::VK_IMAGE_LAYOUT_UNDEFINED },
    _staging      { nullptr },
    _pool         { pool },
    _queue        { queue },
    _instance     { instance }
{
    CONSOLE_INFO("");
}

Texture2D::~Texture2D() {
    CONSOLE_INFO("");

    ::vkDestroyImageView(_instance.logical_device(), _view, nullptr);
    ::vkDestroyImage(_instance.logical_device(), _image_handle, nullptr);
    ::vkFreeMemory(_instance.logical_device(), _device_memory, nullptr);
}