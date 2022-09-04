#include "vklearnin/common.hpp"
#include "vklearnin/Textures/Texture2D.hpp"

#include "vklearnin/CommandStructures/CommandQueues.hpp"
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

    auto staging_name = fmt::format("{}.staging", filepath);

    _staging = new StagingBuffer<uint8_t>(
        image_data,
        image_data + image_size,
        _instance,
        staging_name.c_str()
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

    _mip_levels = static_cast<uint32_t>(std::floor(std::log2(
        std::max(static_cast<float>(width), static_cast<float>(height)))
    )) + 1u;
    CONSOLE_TRACE("Set {} mip levels for '{}'", _mip_levels, filepath);

    _create_image(filepath);
    _upload_texture();
}

// =============================================================================
void Texture2D::init_image_view() {
    CONSOLE_INFO("");

    _image_view = ImageTools::init_view(
        _image_handle,
        _format,
        _mip_levels,
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
                             const vk::Bool32 enable_anisotropy)
{
    CONSOLE_INFO("");

    _sampler.init(min_filter, mag_filter, _mip_levels, mipmap_mode,
                  address_mode_u, address_mode_v, enable_anisotropy,
                  _instance.max_anisotropy());
}

// =============================================================================
void Texture2D::_create_image(const char *alloc_name) {
    CONSOLE_INFO("");

    ImageTools::init_image(
        _extent,
        _format,
        vk::ImageTiling::eOptimal,
        _mip_levels,
        vk::SampleCountFlagBits::e1,
        _image_handle,
        vk::ImageUsageFlagBits::eTransferSrc |
        vk::ImageUsageFlagBits::eTransferDst |
        vk::ImageUsageFlagBits::eSampled,
        _device_memory,
        ::VMA_MEMORY_USAGE_GPU_ONLY,
        ::VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        alloc_name
    );
}

// =============================================================================
void Texture2D::_upload_texture() {
    CONSOLE_INFO("");

    SingleUseCommandBuffer command_buffer(_pool, _instance);
    auto command_buffer_handle = command_buffer.init();
    command_buffer.begin();

    _layout_transition(command_buffer_handle,
                       vk::ImageLayout::eUndefined,
                       vk::ImageLayout::eTransferDstOptimal,
                       0u, _mip_levels);

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

        _generate_mipmaps(command_buffer_handle);

    command_buffer.end();
    command_buffer.submit(_queue);

    delete _staging;
}

// =============================================================================
void Texture2D::_generate_mipmaps(const vk::CommandBuffer &cmd_buffer) {
    CONSOLE_INFO("");

    // first, check to see that the chosen image format supports the blitting
    // vulkan will do for us via CommandBuffer::blitImage()
    auto format_props =
        _instance.physical_device().getFormatProperties(_format);

    if(!(format_props.optimalTilingFeatures &
         vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        CONSOLE_CRITICAL(
            "Texture format {} does not support linear data blitting.",
            to_string(_format)
        );
    }

    CONSOLE_TRACE(
        "Texture format {} supports linear data blitting.",
        to_string(_format)
    );

    int32_t mip_width  = _extent.width;
    int32_t mip_height = _extent.height;

    for(uint32_t level = 1; level < _mip_levels; ++level) {
        CONSOLE_TRACE("Mip level {}", level);

        _layout_transition(
            cmd_buffer,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eTransferSrcOptimal,
            level - 1u
        );

        vk::ImageBlit blit {
            .srcSubresource {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .mipLevel = level - 1,
                .baseArrayLayer = 0u,
                .layerCount = 1u,
            },
            .srcOffsets = std::array<vk::Offset3D, 2> {
                vk::Offset3D { 0, 0, 0 },
                vk::Offset3D { mip_width, mip_height, 1 }
            },
            .dstSubresource {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .mipLevel = level,
                .baseArrayLayer = 0u,
                .layerCount = 1u,
            },
            .dstOffsets = std::array<vk::Offset3D, 2> {
                vk::Offset3D { 0, 0, 0 },
                vk::Offset3D {
                    mip_width  > 1 ? mip_width  / 2 : 1,
                    mip_height > 1 ? mip_height / 2 : 1,
                    1
                }
            },
        };

        cmd_buffer.blitImage(
            _image_handle, vk::ImageLayout::eTransferSrcOptimal,
            _image_handle, vk::ImageLayout::eTransferDstOptimal,
            { blit },
            vk::Filter::eLinear
        );

        _layout_transition(
            cmd_buffer,
            vk::ImageLayout::eTransferSrcOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal,
            level - 1u
        );

        if(mip_width  > 1) { mip_width  /= 2; }
        if(mip_height > 1) { mip_height /= 2; }
    }

    _layout_transition(
        cmd_buffer,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        _mip_levels - 1u
    );
}

// =============================================================================
void Texture2D::_layout_transition(const vk::CommandBuffer &cmd_buffer,
                                   const vk::ImageLayout &old_layout,
                                   const vk::ImageLayout &new_layout,
                                   const uint32_t base_mip_level,
                                   const uint32_t mip_levels)
{
    CONSOLE_INFO("");

    ImageTools::layout_transition(
        cmd_buffer,
        _image_handle,
        base_mip_level,
        mip_levels,
        old_layout,
        new_layout
    );

    _layout = new_layout;
}

// =============================================================================
Texture2D::Texture2D(const CommandQueues &command_queues,
                     const Instance &instance) :
    _offset       { 0, 0, 0 },
    _extent       { 0u, 0u, 1u },
    _sampler      { Sampler2D(instance.logical_device()) },
    _format       { vk::Format::eUndefined },
    _layout       { vk::ImageLayout::eUndefined },
    _mip_levels   { 1u },
    _staging      { nullptr },
    _pool         { command_queues.command_pool() },
    _queue        { command_queues.graphics_queue() },
    _instance     { instance }
{
    CONSOLE_INFO("");
}

Texture2D::~Texture2D() {
    CONSOLE_INFO("");

    _instance.logical_device().destroy(_image_view);
    ImageTools::destroy_image(_image_handle, _device_memory);
}