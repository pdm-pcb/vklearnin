#include "vklearnin/common.hpp"
#include "vklearnin/Textures/Texture2D.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/CommandStructures/SingleUseCommandBuffer.hpp"

#include "stb_image.h"

// =============================================================================
void Texture2D::load_file(const char *filepath, const bool flip_vertical) {
    CONSOLE_INFO("");

    int width;
    int height;
    int channels;

    ::stbi_set_flip_vertically_on_load(flip_vertical);

    uint8_t *image_data = ::stbi_load(
        filepath,
        &width,
        &height,
        &channels,
        BPC::RGBA
    );

    if(image_data == nullptr) {
		CONSOLE_CRITICAL("Failed to load image '{}'\n\t"
                         "Size/Channels: {}x{}@{}\n\t"
                         "Error: '{}'",
                         filepath, width, height, channels,
                         ::stbi_failure_reason());
        return;
    }

    auto image_size = static_cast<size_t>(width * height * BPC::RGBA);

    _staging = new StagingBuffer<uint8_t>(
        image_data,
        image_data + image_size,
        _instance
    );

    ::stbi_image_free(image_data);

    _extent.width  = static_cast<uint32_t>(width);
    _extent.height = static_cast<uint32_t>(height);
    _format = vk::Format::eR8G8B8A8Srgb;

    if(channels != BPC::RGBA) {
        CONSOLE_TRACE(
            "Unsupported image channel count {} for image '{}'; converted "
            "to RGBA",
            channels, filepath
        );
    }

    _create_image();
    _upload_texture();
}

// =============================================================================
void Texture2D::init_image_view() {
    CONSOLE_INFO("");

    _image_view = ImageTools::init_view(
        _image_handle,
        _format,
        vk::ImageAspectFlagBits::eColor,
        _instance.logical_device()
    );
}

// =============================================================================
void Texture2D::init_sampler(const vk::Filter min_filter,
                             const vk::Filter mag_filter,
                             const vk::SamplerMipmapMode mipmap_mode,
                             const vk::SamplerAddressMode address_mode_u,
                             const vk::SamplerAddressMode address_mode_v,
                             const vk::Bool32 enable_anisotropy,
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
        _extent, _format, vk::ImageTiling::eOptimal,
        _image_handle,
        vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled,
        _device_memory,
        ::VMA_MEMORY_USAGE_GPU_ONLY,
        ::VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
    );
}

// =============================================================================
void Texture2D::_upload_texture() {
    CONSOLE_INFO("");

    _layout_transition(vk::ImageLayout::eUndefined,
                       vk::ImageLayout::eTransferDstOptimal);

        SingleUseCommandBuffer command_buffer(_pool, _instance);
        auto command_buffer_handle = command_buffer.init();
        command_buffer.begin();

            vk::BufferImageCopy copy_region {
                .bufferOffset      = 0u,
                .bufferRowLength   = 0u,
                .bufferImageHeight = 0u,
                .imageSubresource {
                    .aspectMask     = vk::ImageAspectFlagBits::eColor,
                    .mipLevel       = 0u,
                    .baseArrayLayer = 0u,
                    .layerCount     = 1u
                },
                .imageOffset = _offset,
                .imageExtent = _extent
            };

            vk::BufferImageCopy copy_regions[] {
                { copy_region }
            };

            command_buffer_handle.copyBufferToImage(
                _staging->handle(),
                _image_handle,
                vk::ImageLayout::eTransferDstOptimal,
                copy_regions
            );

        command_buffer.end();
        command_buffer.submit(_queue);

    _layout_transition(vk::ImageLayout::eTransferDstOptimal,
                       vk::ImageLayout::eShaderReadOnlyOptimal);

    delete _staging;
}

// =============================================================================
void Texture2D::_layout_transition(const vk::ImageLayout &old_layout,
                                   const vk::ImageLayout &new_layout)
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
Texture2D::Texture2D(const vk::CommandPool &pool, const vk::Queue &queue,
                     const Instance &instance) :
    _offset       { 0, 0, 0 },
    _extent       { 0u, 0u, 1u },
    _sampler      { Sampler2D(instance.logical_device()) },
    _format       { vk::Format::eUndefined },
    _layout       { vk::ImageLayout::eUndefined },
    _staging      { nullptr },
    _pool         { pool },
    _queue        { queue },
    _instance     { instance }
{
    CONSOLE_INFO("");
}

Texture2D::~Texture2D() {
    CONSOLE_INFO("");

        CONSOLE_TRACE(
            "Destroying texture image {}",
            fmt::ptr(&_image_handle)
        );

    _instance.logical_device().destroy(_image_view);
    ::vmaDestroyImage(Allocator::allocator(), _image_handle, _device_memory);
}