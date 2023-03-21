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
void Swapchain::submit_and_present(FrameData const &frame) {
    static uint32_t next_image_index = std::numeric_limits<uint32_t>::max();

    auto const acquire_result =
        LogicalDevice::native().acquireNextImageKHR(
            _swapchain,
            std::numeric_limits<uint64_t>::max(),
            frame.acquire_complete_sem(),
            VK_NULL_HANDLE,
            &next_image_index
        );

    if(acquire_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to acquire next swapchain image: '{}'",
            to_string(acquire_result)
        );
        return;
    }

    // Once LogicalDevice has acquired an image for us, it'll signal this
    // semaphore
    vk::Semaphore const acquire_complete_sems[] {
        frame.acquire_complete_sem()
    };

    // We want the image to be fully acquired before beginning to write to it,
    // so if the color attachemnt output stage is reached but the above
    // semaphore  hasn't signaled, wait on it.
    vk::PipelineStageFlags const acquire_before_stage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

    // The command buffer we're getting ready to submit
    vk::CommandBuffer const command_buffers[] {
        frame.command_buffer().native()
    };

    // When it comes time for the presentation engine to take this image back
    // and show it on the display, we'll need a semaphore for it to wait on
    // in case the command buffer hasn't been completely executed.
    vk::Semaphore const commands_complete_sems[] {
        frame.commands_complete_sem()
    };

    vk::SubmitInfo const submit_info[] {
        vk::SubmitInfo { }
            .setWaitSemaphores(acquire_complete_sems)
            .setWaitDstStageMask(acquire_before_stage)
            .setCommandBuffers(command_buffers)
            .setSignalSemaphores(commands_complete_sems)
    };

    // Submit this work to the queue
    LogicalDevice::cmd_queue().native().submit(
        submit_info,
        frame.queue_complete_fence()
    );

    vk::SwapchainKHR const swapchains[] {
        _swapchain
    };

    uint32_t const image_indices[] {
        next_image_index
    };

    auto const submit_result =
        LogicalDevice::cmd_queue().native().presentKHR(
            vk::PresentInfoKHR { }
                .setWaitSemaphores(commands_complete_sems)
                .setSwapchains(swapchains)
                .setImageIndices(image_indices)
        );

    if(submit_result != vk::Result::eSuccess) {
        CONSOLE_WARN("Doooo something.");
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

    CONSOLE_TRACE("Created swapchain for logical device");

    _get_images();
}

// =============================================================================
void Swapchain::destroy() {
    for(auto &image : _images) {
        ImageTools::destroy_view(image);
    }

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
}

// =============================================================================
void Swapchain::_query_surface_format() {
    auto const& gpu = PhysicalDevice::native();
    auto const& surface = TargetWindow::surface();
    auto const& formats = gpu.getSurfaceFormatsKHR(surface);

    CONSOLE_TRACE("Found {} surface formats.", formats.size());

    // First, default to the image format details of the first listed - these
    // are only used if we can't find the desired combo in the for loop below.
    _image_format = formats[0].format;
    _color_space  = formats[0].colorSpace;

    for(auto const& format : formats) {
        if(format.format == vk::Format::eR8G8B8A8Unorm &&
           format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            _image_format = format.format;
            _color_space  = format.colorSpace;
        }

        CONSOLE_TRACE(
            "    {} / {}",
            to_string(format.format),
            to_string(format.colorSpace)
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
        CONSOLE_CRITICAL("Neither immediate nor FIFO presentation modes "
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

    CONSOLE_TRACE(
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