#include "vklearnin/common.hpp"
#include "vklearnin/Buffers/DepthBuffer.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/Swapchain.hpp"

// =============================================================================
void DepthBuffer::init_image(const vk::ImageTiling &tiling,
                             const vk::FormatFeatureFlags &flags)
{
    CONSOLE_INFO("");

    _choose_format(tiling, flags);

    auto[width, height] = _swapchain.extent();

    ImageTools::init_image(
        { width, height, 1u },
        _format, tiling,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        _image_handle, _device_memory,
        _instance
    );
}

// =============================================================================
void DepthBuffer::init_image_view() {
    CONSOLE_INFO("");

    _image_view = ImageTools::init_view(
        _image_handle,
        _format,
        vk::ImageAspectFlagBits::eDepth,
        _instance.logical_device()
    );
}

// =============================================================================
void DepthBuffer::_choose_format(const vk::ImageTiling &tiling,
                                 const vk::FormatFeatureFlags &flags)
{
    CONSOLE_INFO("");

    vk::Format formats[] {
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint,
    };

    size_t fmt_index = 0;
    while(fmt_index < std::size(formats)) {
        auto format_props =_instance.physical_device().getFormatProperties(
            formats[fmt_index]
        );

        if(tiling == vk::ImageTiling::eLinear &&
          (format_props.linearTilingFeatures & flags) == flags)
        {
            break;
        }

        if(tiling == vk::ImageTiling::eOptimal &&
          (format_props.optimalTilingFeatures & flags) == flags)
        {
            break;
        }

        ++fmt_index;
    }

    if(fmt_index == std::size(formats)) {
        CONSOLE_CRITICAL("Could not find suitable depth buffer format.");
    }

    _format = formats[fmt_index];
}

// =============================================================================
DepthBuffer::DepthBuffer(const Instance &instance, const Swapchain &swapchain) :
    _image_handle  { nullptr },
    _device_memory { nullptr },
    _image_view    { nullptr },
    _format        { vk::Format::eUndefined },
    _instance      { instance  },
    _swapchain     { swapchain }
{
    CONSOLE_INFO("");
}

DepthBuffer::~DepthBuffer() {
    CONSOLE_INFO("");

    ::vkDestroyImage(_instance.logical_device(),     _image_handle,  nullptr);
    ::vkDestroyImageView(_instance.logical_device(), _image_view,    nullptr);
    ::vkFreeMemory(_instance.logical_device(),       _device_memory, nullptr);
}