#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Swapchain.hpp"

#include "vklearnin/rendering/PhysicalDevice.hpp"
#include "vklearnin/rendering/LogicalDevice.hpp"
#include "vklearnin/rendering/DeviceQueue.hpp"

namespace vkl {

//==============================================================================
void Swapchain::create() {
    _query_surface_capabilities();
    _query_surface_format();
    _query_surface_present_modes();
    _set_create_info();

    auto result = _logical_device.native().createSwapchainKHR(_create_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create swapchain");
    }
    _swapchain = result.value;
    CONSOLE_TRACE("Created swapchain for logical device");

    _get_images();
    _create_image_views();
}

//==============================================================================
void Swapchain::destroy() {
    for(size_t image = 0; image < RenderConfig::swapchain_image_count; ++image)
    {
        ImageTools::destroy_view(
            _image_views[image],
            _logical_device.native()
        );
    }
    
    _logical_device.native().destroy(_swapchain);
}

//==============================================================================
uint32_t Swapchain::next_image_index(const vk::Semaphore &semaphore) {
    // grab the next swapchain image and check it...
    auto result = _logical_device.native().acquireNextImageKHR(
        { _swapchain },
        std::numeric_limits<int64_t>::max(),
        semaphore,
        nullptr,
        &_current_image_index
    );

    if(result == vk::Result::eErrorOutOfDateKHR ||
       result == vk::Result::eSuboptimalKHR)
    {
        CONSOLE_WARN("acquireNextImageKHR() returned '{}'", to_string(result));
        _current_image_index = std::numeric_limits<uint32_t>::max();
    }
    else if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "acquireNextImageKHR() failed with '{}'",
            to_string(result)
        );
    }

    return _current_image_index;
}

//==============================================================================
void Swapchain::_query_surface_capabilities() {
    const auto &vk_pdev = PhysicalDevice::current().native();
    const auto &result = vk_pdev.getSurfaceCapabilitiesKHR(_surface);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not get surface capabilities.");
    }
    const auto &capabilities = result.value;

    CONSOLE_TRACE(
        "\nSurface Capabilities:"
        "\n\t Minimum Image Count: {}"
        "\n\t Maximum Image Count: {}"
        "\n\t Current Extent: {} x {}"
        "\n\t Minimum Extent: {} x {}"
        "\n\t Maximum Extent: {} x {}"
        "\n\t Maximum Image Array Layers: {}",
        capabilities.minImageCount,
        capabilities.maxImageCount,
        capabilities.currentExtent.width,
        capabilities.currentExtent.height,
        capabilities.minImageExtent.width,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.width,
        capabilities.maxImageExtent.height,
        capabilities.maxImageArrayLayers
    );

    // The vk::SurfaceCapabilitiesKHR struct will let us know what resolutions
    // our surface is allowed to be.
    _extent.width = std::clamp(
        static_cast<uint32_t>(RenderConfig::window_width),
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width
    );
    _extent.height = std::clamp(
        static_cast<uint32_t>(RenderConfig::window_height),
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height
    );

    if(_extent.width != RenderConfig::window_width ||
       _extent.height != RenderConfig::window_height)
    {
        CONSOLE_WARN(
            "Requested resolution {}x{} unsupported; using {}x{} instead",
            RenderConfig::window_width, RenderConfig::window_height,
            _extent.width, _extent.height
        );
    }

    // Provided swapchain_image_count has already been used to set some array
    // sizes (in LogicalDevice, for example) it's become a hard requirement of
    // the surface
    if(RenderConfig::swapchain_image_count > capabilities.maxImageCount ||
       RenderConfig::swapchain_image_count < capabilities.minImageCount)
    {
        CONSOLE_CRITICAL(
            "{} swapchain images requested, but surface allows a minimum of {} "
            "and a maximum of {} images",
            RenderConfig::swapchain_image_count,
            capabilities.minImageCount,
            capabilities.maxImageCount
        );
    }
}

//==============================================================================
void Swapchain::_query_surface_format() {
    const auto &vk_pdev = PhysicalDevice::current().native();
    const auto &fmt_result  = vk_pdev.getSurfaceFormatsKHR(_surface);
    if(fmt_result.result != vk::Result::eSuccess ||
       fmt_result.value.size() == 0)
    {
        CONSOLE_CRITICAL("Could not get surface formats.");
    }
    const auto &formats = fmt_result.value;
    CONSOLE_TRACE("Found {} surface formats.", formats.size());

    // First, default to the image format details of the first listed - these
    // are only used if we can't find the desired combo in the for loop below.
    _surface_format = formats[0].format;
    _color_space    = formats[0].colorSpace;

    for(const auto &format : formats) {
        if(format.format == vk::Format::eB8G8R8A8Srgb &&
           format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            _surface_format = format.format;
            _color_space    = format.colorSpace;
        }

        CONSOLE_TRACE(
            "    {} / {}",
            to_string(format.format),
            to_string(format.colorSpace)
        );
    }
}

//==============================================================================
void Swapchain::_query_surface_present_modes() {
    const auto &vk_pdev = PhysicalDevice::current().native();
    const auto &mode_result = vk_pdev.getSurfacePresentModesKHR(_surface);
    if(mode_result.result != vk::Result::eSuccess ||
       mode_result.value.size() == 0)
    {
        CONSOLE_CRITICAL("Could not get surface present modes.");
    }

    const auto &modes = mode_result.value;
    CONSOLE_TRACE("Found {} present modes.", modes.size());

    bool has_fifo_relaxed = false;
    bool has_fifo         = false;
    bool has_immediate    = false;

    // iterate available modes, noting what we've got
    for(const auto mode : modes) {
        CONSOLE_TRACE("    {}", to_string(mode));
        if(mode == vk::PresentModeKHR::eFifoRelaxed) {
            has_fifo_relaxed = true;
        }
        else if(mode == vk::PresentModeKHR::eFifo) {
            has_fifo = true;
        }
        else if(mode == vk::PresentModeKHR::eImmediate) {
            has_immediate = true;
        }
    }

    if(has_fifo_relaxed && RenderConfig::vsync_on) {
        _present_mode = vk::PresentModeKHR::eFifoRelaxed;
    }
    else if(has_fifo && RenderConfig::vsync_on) {
        _present_mode = vk::PresentModeKHR::eFifo;
    }
    else if(RenderConfig::vsync_on) {
        CONSOLE_WARN("VSync requested but the available present modes don't "
                     "support it.");
    }
    else if(has_immediate) {
        _present_mode = vk::PresentModeKHR::eImmediate;
    }
    else {
        CONSOLE_CRITICAL("Neither immediate nor FIFO presentation modes "
                         "supported.");
    }

    CONSOLE_TRACE("Assigning presentation mode {}", to_string(_present_mode));
}

void Swapchain::_set_create_info() {
    // Now we're ready to fill out this struct
    _create_info = {
        .surface         = _surface,
        .minImageCount   = RenderConfig::swapchain_image_count,
        .imageFormat     = _surface_format,
        .imageColorSpace = _color_space,
        .imageExtent     = _extent,
        // Image array layers will always be one, except in the case of a VR
        // headset or other stereoscopic display
        .imageArrayLayers = 1u,
        // Marking the images in this swapchain as color attachments means they
        // can be used to draw the scene into
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        // Transforms are what you'd expect: the image can be flipped, rotated,
        // etc. Leaving it alone gets us what we want for now.
        .preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,
        // Working with transparency and blending will come later
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        // Store the selected presentation mode
        .presentMode = _present_mode,
        // Setting clipped to true allows Vulkan to ignore pixels on the render
        // surface that can't be seen. For example, moving a window in front
        // of the render surface.
        .clipped = true,
        // There are situations during which you'll want to recreate the
        // swapchain from scratch. Providing the previous swapchain aids this
        // process.
        .oldSwapchain = nullptr,
    };

    // In the event that the graphics and present queues are in different
    // families on this device, we'll bail for the time being
    auto &dev_queues = _logical_device.queues();

    if(dev_queues.size() == 1) {
        // All is well and we don't have to worry about sharing this swapchain
        // between separate device queues.
        _create_info.imageSharingMode = vk::SharingMode::eExclusive;
        _create_info.queueFamilyIndexCount = 0;
        _create_info.pQueueFamilyIndices = nullptr;
        CONSOLE_TRACE("Swapchain being created with exclusive sharing mode.");
    }
    else {
        // Looks like there will be sharing required, so let the swapchain know
        // it'll have to pay attention.
        CONSOLE_CRITICAL("Swapchains using concurrent sharing mode are "
                         "unsuspported");
    }

    CONSOLE_TRACE(
        "\nSwapchain Create Info:"
        "\n    Extent:       {}x{}"
        "\n    Image Count:  {}"
        "\n    Format:       {}"
        "\n    Color Space:  {}"
        "\n    Present Mode: {}",
        _extent.width, _extent.height,
        RenderConfig::swapchain_image_count,
        to_string(_surface_format),
        to_string(_color_space),
        to_string(_present_mode)
    );
}

//==============================================================================
void Swapchain::_get_images() {
    auto result = _logical_device.native().getSwapchainImagesKHR(_swapchain);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not get swapchain images");
    }
    _images = result.value;

    if(_images.size() != RenderConfig::swapchain_image_count) {
        CONSOLE_CRITICAL(
            "Swapchain returned {} images; {} requested",
            _images.size(), RenderConfig::swapchain_image_count
        );
    }
}

//==============================================================================
void Swapchain::_create_image_views() {
    _image_views.resize(RenderConfig::swapchain_image_count);    
    for(size_t image = 0; image < RenderConfig::swapchain_image_count; ++image)
    {
        _image_views[image] = ImageTools::create_view(
            _images[image],
            _surface_format,
            vk::ImageAspectFlagBits::eColor,
            _logical_device.native()
        );
    }

    CONSOLE_TRACE("Created {} swapchain image views", _image_views.size());
}

//==============================================================================
Swapchain::Swapchain(LogicalDevice  &logical_device,
                     const vk::SurfaceKHR &surface) :
    _create_info         { },
    _swapchain           { },
    _surface_format      { vk::Format::eUndefined },
    _color_space         { vk::ColorSpaceKHR::eSrgbNonlinear },
    _current_image_index { 0u },
    _offset              { 0, 0 },
    _extent              { std::numeric_limits<uint32_t>::max(),
                           std::numeric_limits<uint32_t>::max() },
    _present_mode        { vk::PresentModeKHR::eImmediate },
    _logical_device      { logical_device  },
    _surface             { surface }
{ }

} // namespace vkl