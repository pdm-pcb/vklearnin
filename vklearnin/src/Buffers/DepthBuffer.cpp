#include "vklearnin/common.hpp"
#include "vklearnin/Buffers/DepthBuffer.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/Swapchain.hpp"

// =============================================================================
void DepthBuffer::init_image(const ::VkImageTiling &tiling,
                             const ::VkFormatFeatureFlags &flags)
{
    CONSOLE_INFO("");

    _choose_format(tiling, flags);

    auto[width, height] = _swapchain.extent();

    ImageTools::init_image(
        { width, height, 1u },
        _format, tiling,
        ::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        ::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
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
        ::VK_IMAGE_ASPECT_DEPTH_BIT,
        _instance.logical_device()
    );
}

// =============================================================================
void DepthBuffer::_choose_format(const ::VkImageTiling &tiling,
                                 const ::VkFormatFeatureFlags &flags)
{
    CONSOLE_INFO("");

    ::VkFormat formats[] {
        ::VK_FORMAT_D32_SFLOAT,
        ::VK_FORMAT_D32_SFLOAT_S8_UINT,
        ::VK_FORMAT_D24_UNORM_S8_UINT
    };

    size_t fmt_index = 0;
    while(fmt_index < std::size(formats)) {
        ::VkFormatProperties format_props;
        ::vkGetPhysicalDeviceFormatProperties(
            _instance.physical_device(),
            formats[fmt_index],
            &format_props
        );

        if(tiling == ::VK_IMAGE_TILING_LINEAR &&
          (format_props.linearTilingFeatures & flags) == flags)
        {
            break;
        }

        if(tiling == ::VK_IMAGE_TILING_OPTIMAL &&
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
    _format        { ::VK_FORMAT_UNDEFINED },
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