#include "vklearnin/vklearnin.hpp"
#include "vklearnin/shaders/Texture2D.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"

#include <stb/stb_image.h>

namespace vkl {

//==============================================================================
void Texture2D::load_from_file(const char *filepath, const bool flip_vertical)
{
    int width;
    int height;
    int channels;

    ::stbi_set_flip_vertically_on_load(flip_vertical);
    ::stbi_uc *image_data = ::stbi_load(
        filepath,
        &width,
        &height,
        &channels,
        ::STBI_rgb_alpha
    );

    if(image_data == nullptr) {
		CONSOLE_CRITICAL("Failed to load image '{}'\n\t"
                         "Size/Channels: {}x{}@{}\n\t"
                         "Error: '{}'",
                         filepath, _width, _height, _channels,
                         ::stbi_failure_reason());
        return;
    }

    _width = static_cast<uint32_t>(width);
    _height = static_cast<uint32_t>(height);
    _channels = static_cast<uint32_t>(channels);

    _image = ImageTools::create_image(
        {
            .width  = static_cast<uint32_t>(_width),
            .height = static_cast<uint32_t>(_height),
            .depth  = 1u
        },
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageAspectFlagBits::eColor,
        vk::ImageTiling::eOptimal,
        1u,
        vk::SampleCountFlagBits::e1,
        (vk::ImageUsageFlagBits::eTransferDst |
         vk::ImageUsageFlagBits::eSampled),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    _size_bytes = _width * _height * ::STBI_rgb_alpha;
    auto staging_buffer = BufferTools::stage_data(_size_bytes, image_data);
    ImageTools::move_to_device(staging_buffer, _image, { _width, _height, 1u });

    ::stbi_image_free(image_data);
    BufferTools::destroy_buffer(staging_buffer);

    _image.sampler = ImageTools::create_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eClampToBorder
    );
}

//==============================================================================
void Texture2D::destroy() {
    LogicalDevice::native().destroy(_image.image);
    LogicalDevice::native().destroy(_image.view);
    LogicalDevice::native().freeMemory(_image.memory);
    LogicalDevice::native().destroy(_image.sampler);
}

//==============================================================================
Texture2D::Texture2D() :
    _width    { 0u },
    _height   { 0u },
    _channels { 0u }
{ }

} // namespace vkl