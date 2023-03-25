#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"

#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/system/window/TargetWindow.hpp"
#include "vklearnin/rendering/FrameData.hpp"

namespace vkl {

vk::Extent2D       Swapchain::_extent { 0u, 0u };
vk::Offset2D       Swapchain::_offset { 0, 0 };
vk::Rect2D         Swapchain::_render_area;

vk::Format         Swapchain::_image_format = vk::Format::eUndefined;
vk::ColorSpaceKHR  Swapchain::_color_space  = vk::ColorSpaceKHR::eSrgbNonlinear;
vk::PresentModeKHR Swapchain::_present_mode = vk::PresentModeKHR::eImmediate;

vk::SwapchainKHR Swapchain::_swapchain;

std::vector<ImageObject> Swapchain::_images;

// =============================================================================
void Swapchain::acquire_next_image_index(FrameData &frame) {
    uint32_t next_image_index = std::numeric_limits<uint32_t>::max();

    auto const result =
        LogicalDevice::native().acquireNextImageKHR(
            _swapchain,
            std::numeric_limits<uint64_t>::max(),
            frame.acquire_complete_sem(),
            VK_NULL_HANDLE,
            &next_image_index
        );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to acquire next swapchain image: '{}'",
            to_string(result)
        );
    }

    frame.set_image_index(next_image_index);
}

// =============================================================================
void Swapchain::present(FrameData const &frame) {
    // This array must be a duplicate of the one we used when submitting this
    // frame's command buffer to the GPU
    vk::Semaphore const commands_complete_sems[] {
        frame.commands_complete_sem()
    };

    // There's one swapchain to present from
    vk::SwapchainKHR const swapchains[] { _swapchain };

    // The swapchain image index associated with this frame's command buffer
    auto const image_index = frame.image_index();

    // Build the submit info struct
    vk::PresentInfoKHR const present_info {
        .waitSemaphoreCount = static_cast<uint32_t>(
            std::size(commands_complete_sems)
        ),
        .pWaitSemaphores = commands_complete_sems,
        .swapchainCount  = static_cast<uint32_t>(std::size(swapchains)),
        .pSwapchains     = swapchains,
        .pImageIndices   = &image_index,
    };

    auto const result =
        LogicalDevice::cmd_queue().native().presentKHR(present_info);

    if(result == vk::Result::eSuboptimalKHR ||
       result == vk::Result::eErrorOutOfDateKHR)
    {
        // TODO: this.
        CONSOLE_WARN("Recreate swapchain.");
    }
    else if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Swapchain image failed: '{}", to_string(result));
    }
}

// =============================================================================
void Swapchain::create() {
    _query_surface_capabilities();
    _query_surface_format();
    _query_surface_present_modes();

    vk::SwapchainCreateInfoKHR create_info { };
    _populate_create_info(create_info);

    _swapchain = LogicalDevice::native().createSwapchainKHR(create_info);

    CONSOLE_TRACE(
        "Created swapchain {:#x}",
        reinterpret_cast<uint64_t>(VkSwapchainKHR(_swapchain))
    );

    _get_images();
}

// =============================================================================
void Swapchain::destroy() {
    for(auto &image : _images) {
        ImageTools::destroy_view(image);
    }


    CONSOLE_TRACE(
        "Destroying swapchain {:#x}",
        reinterpret_cast<uint64_t>(VkSwapchainKHR(_swapchain))
    );

    LogicalDevice::native().destroy(_swapchain);
}

// =============================================================================
void Swapchain::_query_surface_capabilities() {
    auto const& gpu = PhysicalDevice::native();
    auto const& surface = TargetWindow::surface();
    auto const& capabilities = gpu.getSurfaceCapabilitiesKHR(surface);

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

    _render_area = vk::Rect2D {
        .offset = _offset,
        .extent = _extent,
    };

    // Provided image count has already been used to set some array sizes (in
    // LogicalDevice, for example) it's become a hard requirement of the
    // surface itself
    if(RenderConfig::swapchain_image_count > capabilities.maxImageCount ||
       RenderConfig::swapchain_image_count < capabilities.minImageCount)
    {
        CONSOLE_WARN(
            "{} swapchain images requested, but surface allows a minimum of {} "
            "and a maximum of {} images. Defaulting to minimum.",
            RenderConfig::swapchain_image_count,
            capabilities.minImageCount,
            capabilities.maxImageCount
        );

        RenderConfig::swapchain_image_count = capabilities.minImageCount;
    }

    if(RenderConfig::swapchain_image_count == 2 && RenderConfig::vsync_on) {
        if(capabilities.maxImageCount >= 3) {
            RenderConfig::swapchain_image_count = 3;
        }
    }
}

// =============================================================================
void Swapchain::_query_surface_format() {
    auto const& gpu = PhysicalDevice::native();
    auto const& surface = TargetWindow::surface();
    auto const& formats = gpu.getSurfaceFormatsKHR(surface);

    CONSOLE_TRACE("Found {} surface formats.", formats.size());

    bool found_desired = false;
    auto const desired_format = vk::Format::eB8G8R8A8Unorm;
    auto const deisred_space = vk::ColorSpaceKHR::eSrgbNonlinear;

    for(auto const& format : formats) {
        if(format.format == desired_format &&
           format.colorSpace == deisred_space)
        {
            _image_format = format.format;
            _color_space  = format.colorSpace;
            found_desired = true;
        }

        CONSOLE_TRACE(
            "    {} / {}",
            to_string(format.format),
            to_string(format.colorSpace)
        );
    }

    if(!found_desired) {
        _image_format = formats[0].format;
        _color_space  = formats[0].colorSpace;

        CONSOLE_WARN(
            "Could not find desired swapchain surface format/color space of "
            "{} / {}. Defaulting instead to {} / {}.",
            to_string(desired_format),
            to_string(deisred_space),
            to_string(_image_format),
            to_string(_color_space)
        );
    }
}

// =============================================================================
void Swapchain::_query_surface_present_modes() {
    auto const& gpu = PhysicalDevice::native();
    auto const& surface = TargetWindow::surface();
    auto const& modes = gpu.getSurfacePresentModesKHR(surface);

    CONSOLE_TRACE("Found {} present modes.", modes.size());

    bool has_fifo_relaxed = false;
    bool has_fifo         = false;
    bool has_immediate    = false;

    // iterate available modes, noting what we've got
    for(auto const mode : modes) {
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
        CONSOLE_CRITICAL("Neither immediate nor FIFO presentation modes are "
                         "supported.");
    }
}

// =============================================================================
void Swapchain::_populate_create_info(vk::SwapchainCreateInfoKHR &create_info) {
    create_info = {
        .surface         = TargetWindow::surface(),
        .minImageCount   = RenderConfig::swapchain_image_count,
        .imageFormat     = _image_format,
        .imageColorSpace = _color_space,
        .imageExtent     = _extent,

        // Image array layers will always be one, except in the case of a
        // device with displays interested in the same swapchain, like a VR
        // headset
        .imageArrayLayers = 1u,

        // Marking the images in this swapchain as color attachments means they
        // can be used to draw the scene into
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,

        // We don't need to worry about sharing images between queues
        .imageSharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,

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
        .clipped = 1u,

        // There are situations during which you'll want to recreate the
        // swapchain from scratch. Providing the previous swapchain aids this
        // process.
        .oldSwapchain = nullptr,
    };

    CONSOLE_INFO(
        "\nSwapchain Create Info:"
        "\n    Extent:       {}x{}"
        "\n    Image Count:  {}"
        "\n    Format:       {}"
        "\n    Color Space:  {}"
        "\n    Present Mode: {}",
        _extent.width, _extent.height,
        RenderConfig::swapchain_image_count,
        to_string(_image_format),
        to_string(_color_space),
        to_string(_present_mode)
    );
}

// =============================================================================
void Swapchain::_get_images() {
    auto const &image_handles =
        LogicalDevice::native().getSwapchainImagesKHR(_swapchain);

    if(image_handles.size() != RenderConfig::swapchain_image_count) {
        CONSOLE_CRITICAL(
            "Swapchain supports {} images; {} configured",
            _images.size(),
            RenderConfig::swapchain_image_count
        );
        return;
    }

    CONSOLE_INFO("Acquired {} swapchain images", image_handles.size());

    _images.reserve(RenderConfig::swapchain_image_count);

    for(auto const &image_handle : image_handles) {
        _images.emplace_back(ImageObject{
            .format = _image_format,
            .layout =  vk::ImageLayout::eUndefined,
            .handle = image_handle,
        });

        ImageTools::create_view(
            _images.back(),
            vk::ImageViewType::e2D,
            vk::ImageAspectFlagBits::eColor
        );
    }
}

} // namespace vkl