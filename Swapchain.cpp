#include "common.hpp"
#include "Swapchain.hpp"

#include "Instance.hpp"
#include "CommandQueues.hpp"

#if defined(__linux__)
    #include "X11Window.hpp"
#elif defined(_WIN32)
    #include "Win32Window.hpp"
#endif

//==============================================================================
void Swapchain::init_color_format() {
    CONSOLE_INFO("");

    // query and populate the list of available color spaces/image formats
    uint32_t format_count = 0u;
    ::VkResult result = _instance._GetPhysicalDeviceSurfaceFormatsKHR(
        _instance.physical_device(),
        _window.surface(),
        &format_count,
        nullptr
    );

    if(result != VK_SUCCESS || format_count == 0u) {
        CONSOLE_CRITICAL("Could not query surface format.");
    }

    CONSOLE_TRACE("Found {} color formats", format_count);

    std::vector<::VkSurfaceFormatKHR> formats(format_count);
    result = _instance._GetPhysicalDeviceSurfaceFormatsKHR(
        _instance.physical_device(),
        _window.surface(),
        &format_count,
        formats.data()
    );

    // presumably, we want 32-bit SRGB
    for(const auto &format : formats) {
        if(format.format == ::VK_FORMAT_B8G8R8A8_SRGB &&
           format.colorSpace == ::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            _color_format = format.format;
            _color_space = format.colorSpace;
            CONSOLE_TRACE("32-bit SRGB assigned");
        }
    }

    // otherwise... just take the first available format?
    // TODO: it'd be nice to actually have strings for all of these...
    if(_color_format == ::VK_FORMAT_MAX_ENUM ||
       _color_space == ::VK_COLOR_SPACE_MAX_ENUM_KHR)
    {
        _color_format = formats[0].format;
        _color_space = formats[0].colorSpace;
        CONSOLE_WARN("Assigned default color space.");
    }
}

//==============================================================================
void Swapchain::init_present_modes() {
    CONSOLE_INFO("");

    // query and populate the list of presentation modes
    uint32_t mode_count = 0;
    ::VkResult result = _instance._GetPhysicalDeviceSurfacePresentModesKHR(
        _instance.physical_device(),
        _window.surface(),
        &mode_count,
        nullptr
    );

    if(result != ::VK_SUCCESS || mode_count == 0) {
        CONSOLE_CRITICAL("Unable to query surface presentation modes.");
    }

    CONSOLE_TRACE("Found {} presentation modes", mode_count);

    std::vector<::VkPresentModeKHR> modes(mode_count);
    result = _instance._GetPhysicalDeviceSurfacePresentModesKHR(
        _instance.physical_device(),
        _window.surface(),
        &mode_count,
        modes.data()
    );

    if(result != ::VK_SUCCESS || mode_count == 0) {
        CONSOLE_CRITICAL("Unable to populate surface presentation modes.");
    }

    // iterate available modes, and choose FIFO/V-Sync by default, if it's
    // available
    for(uint32_t mode_index = 0; mode_index < mode_count; ++mode_index) {
        const char *mode;
        switch(modes[mode_index]) {
            case ::VK_PRESENT_MODE_IMMEDIATE_KHR:
                mode = "Immediate";
                break;
            case ::VK_PRESENT_MODE_MAILBOX_KHR:
                mode = "Mailbox";
                break;
            case ::VK_PRESENT_MODE_FIFO_KHR:
                mode = "FIFO";
                break;
            case ::VK_PRESENT_MODE_FIFO_RELAXED_KHR:
                mode = "FIFO Relaxed";
                break;
            case ::VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
                mode = "Shared Demand Refresh";
                break;
            case ::VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
                mode = "Shared Continuous Refresh";
                break;
            default:
                mode = "Unknown";
                assert(false);
                break;
        }

        CONSOLE_TRACE("Presentation mode {}: {}", mode_index + 1, mode);

        if(modes[mode_index] == ::VK_PRESENT_MODE_FIFO_KHR) {
            _present_mode = modes[mode_index];
        }
    }

    // default to immediate/unsynchronized presentations if there's nothing
    // else
    if(_present_mode == ::VK_PRESENT_MODE_MAX_ENUM_KHR) {
        _present_mode = ::VK_PRESENT_MODE_IMMEDIATE_KHR;
        CONSOLE_WARN("Assigning default immedate presentation mode.");
    }
    else {
        CONSOLE_TRACE("FIFO chosen");
    }
}

//==============================================================================
void Swapchain::init_extent() {
    CONSOLE_INFO("");

    ::VkSurfaceCapabilitiesKHR surface_capabilities { };

    ::VkResult result = _instance._GetPhysicalDeviceSurfaceCapabilitiesKHR(
        _instance.physical_device(),
        _window.surface(),
        &surface_capabilities
    );

    if(result != ::VK_SUCCESS || surface_capabilities.maxImageCount == 0) {
        CONSOLE_CRITICAL("Could not get surface capabilities.");
    }

    CONSOLE_TRACE(
        "\nSurface Capabilities:"
        "\n\t Minimum Image Count: {}"
        "\n\t Maximum Image Count: {}"
        "\n\t Current Extent: {} x {}"
        "\n\t Minimum Extent: {} x {}"
        "\n\t Maximum Extent: {} x {}"
        "\n\t Maximum Image Array Layers: {}",
        surface_capabilities.minImageCount,
        surface_capabilities.maxImageCount,
        surface_capabilities.currentExtent.width,
        surface_capabilities.currentExtent.height,
        surface_capabilities.minImageExtent.width,
        surface_capabilities.minImageExtent.height,
        surface_capabilities.maxImageExtent.width,
        surface_capabilities.maxImageExtent.height,
        surface_capabilities.maxImageArrayLayers
    );

    // establish one more than the minimum required images so it's less likely
    // we'll have to wait on the driver in order to have a fresh image to which
    // we can write
    _image_count = surface_capabilities.minImageCount + 1u;

    // Don't exceed the maximum image count, however. Zero is a special maximum
    // indicating unlimiated images
    if(surface_capabilities.maxImageCount != 0u &&
       _image_count >= surface_capabilities.maxImageCount)
    {
        _image_count = surface_capabilities.maxImageCount;
    }

    // we're only asking for one right now, but just make sure
    if(_image_array_layers > surface_capabilities.maxImageArrayLayers) {
        _image_array_layers = surface_capabilities.maxImageArrayLayers;
    }

    // just in case what the surface reported back doesn't make sense, we know
    // what resolution we requested
    if(surface_capabilities.currentExtent.width == UI32MAX ||
       surface_capabilities.currentExtent.height == UI32MAX)
    {
        _extent.width  = _window.x_res();
        _extent.height = _window.y_res();
    }
    else {
        _extent = surface_capabilities.currentExtent;
    }

    if(surface_capabilities.currentTransform ==
       ::VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR)
    {
        CONSOLE_CRITICAL("Could not get surface current transform.");
    }

    // this ought to be the identity transform, I suspect
    _transform = surface_capabilities.currentTransform;
}

//==============================================================================
void Swapchain::init_swapchain(const CommandQueues &queue) {
    CONSOLE_INFO("");

    // now we've got everything we need to actually create the swapchain
    ::VkSwapchainCreateInfoKHR swapchain_info { };
    swapchain_info.sType = ::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface          = _window.surface();
    swapchain_info.minImageCount    = _image_count;
    swapchain_info.imageFormat      = _color_format;
    swapchain_info.imageColorSpace  = _color_space;
    swapchain_info.imageExtent      = _extent;
    swapchain_info.imageArrayLayers = _image_array_layers;
    swapchain_info.preTransform     = _transform;
    swapchain_info.presentMode      = _present_mode;

    uint32_t indices[] = {
        queue.graphics_index(),
        queue.present_index()
    };

    // the queue families should be the same, but if they're not, then the
    // difference must be explicitly noted and a concurrent image sharing mode
    // used
    if(queue.graphics_index() != queue.present_index())
    {
        swapchain_info.imageSharingMode = ::VK_SHARING_MODE_CONCURRENT;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.pQueueFamilyIndices = indices;
        CONSOLE_TRACE("Swapchain being created with concurrent sharing mode.");
    }
    else {
        swapchain_info.imageSharingMode = ::VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.queueFamilyIndexCount = 0;
        swapchain_info.pQueueFamilyIndices = nullptr;
        CONSOLE_TRACE("Swapchain being created with exclusive sharing mode.");
    }
    
    // color attachment denotes immediate rendering, while setting composite
    // alpha to opaque simply disables any blending
    swapchain_info.imageUsage     = ::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_info.compositeAlpha = ::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    // don't render pixels on the surface which are obscured by eg, another
    // window
    swapchain_info.clipped = VK_TRUE;

    // since this is the first time we're creating a swapchain for the
    // application, there's no previous handle
    swapchain_info.oldSwapchain = VK_NULL_HANDLE;

    ::VkResult result = _instance._CreateSwapchainKHR(
        _instance.logical_device(),
        &swapchain_info,
        nullptr,
        &_swapchain
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Could not create swapchain.");
    }
}

//==============================================================================
void Swapchain::init_swapchain_images() {
    CONSOLE_INFO("");

    // query and populate the list of swapchain images
    ::VkResult result = _instance._GetSwapchainImagesKHR(
        _instance.logical_device(),
        _swapchain,
        &_image_count,
        nullptr
    );

    if(result != ::VK_SUCCESS || _image_count == 0) {
        CONSOLE_CRITICAL("Unable to query swapchain images.");
    }

    // in a double buffered setup, there should be two images in the chain
    CONSOLE_TRACE("Swapchain consists of {} images", _image_count);

    _images.resize(_image_count);

    result = _instance._GetSwapchainImagesKHR(
        _instance.logical_device(),
        _swapchain,
        &_image_count,
        _images.data()
    );

    if(result != ::VK_SUCCESS || _image_count == 0) {
        CONSOLE_CRITICAL("Unable to populate swapchain images.");
    }
}

//==============================================================================
void Swapchain::init_image_views() {
    CONSOLE_INFO("");

    // since we now know how many images there are
    _image_views.resize(_image_count);

    // set everything to simple defaults
    for(size_t image_index = 0; image_index < _image_count; ++image_index) {
        ::VkImageViewCreateInfo image_info { };
        image_info.sType      = ::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_info.image      = _images[image_index];
        image_info.viewType   = ::VK_IMAGE_VIEW_TYPE_2D;
        image_info.format     = _color_format;
        image_info.components = {
            .r = ::VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = ::VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = ::VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = ::VK_COMPONENT_SWIZZLE_IDENTITY,
        };
        image_info.subresourceRange.aspectMask = ::VK_IMAGE_ASPECT_COLOR_BIT;
        image_info.subresourceRange.baseMipLevel   = 0u;
        image_info.subresourceRange.levelCount     = 1u;
        image_info.subresourceRange.baseArrayLayer = 0u;
        image_info.subresourceRange.layerCount     = 1u;

        ::VkResult result = ::vkCreateImageView(
            _instance.logical_device(),
            &image_info,
            nullptr,
            &_image_views[image_index]
        );

        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Failed to create image view {}.", image_index);
        }
        else {
            CONSOLE_TRACE("Created image view {}.", image_index);
        }
    }
}

//==============================================================================
#if defined(__linux__)
    Swapchain::Swapchain(const Instance &instance, const X11Window &window) :
#elif defined(_WIN32)
    Swapchain::Swapchain(const Instance &instance, const Win32Window &window) :
#endif
    _instance           { instance },
    _window             { window },
    _color_format       { ::VK_FORMAT_MAX_ENUM },
    _color_space        { ::VK_COLOR_SPACE_MAX_ENUM_KHR },
    _image_count        { 0u },
    _image_array_layers { 1u }, // layers > 1 is for stereoscopic 3D
    _extent             { UI32MAX, UI32MAX },
    _offset             { 0, 0 },
    _present_mode       { ::VK_PRESENT_MODE_MAX_ENUM_KHR },
    _transform          { ::VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR },
    _swapchain          { nullptr }
{
    CONSOLE_INFO("");
}

Swapchain::~Swapchain() {
    CONSOLE_INFO("");
    for(auto view : _image_views) {
        ::vkDestroyImageView(_instance.logical_device(), view, nullptr);
    }

    _instance._DestroySwapchainKHR(_instance.logical_device(), _swapchain, nullptr);
}