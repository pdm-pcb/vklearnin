#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/Swapchain.hpp"

#include "vklearnin/rendering/devices/PhysicalDevice.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/engine/Pipeline.hpp"
#include "vklearnin/rendering/devices/CmdQueue.hpp"

#include "vklearnin/system/TargetWindow.hpp"

namespace vkl {

#if defined(__linux__)
    using TargetWindow = XCBTargetWindow;
#elif defined(_WIN32)
    using TargetWindow = Win32TargetWindow;
#endif

// =============================================================================
vk::Result Swapchain::next_image(const uint32_t frame_index) {
    // wait for an image to become available to write to
    auto result = LogicalDevice::native().waitForFences(
        1u, // fence count
        &_present_fences[frame_index], // which fences to wait on
        VK_TRUE, // signal on all or any of the fences?
        std::numeric_limits<int64_t>::max() // block effectively forever
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("vk::Device::waitForFences() failed");
    }

    // since the fence signaled, we can now ask the swapchain which image it'd
    // like us to write to
    result = LogicalDevice::native().acquireNextImageKHR(
        { _swapchain },
        std::numeric_limits<int64_t>::max(),
        _image_available_sems[frame_index],
        nullptr,
        &_image_index
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_WARN(
            "acquireNextImageKHR() failed with '{}'",
            to_string(result)
        );
    }

    return result;
}

// =============================================================================
void Swapchain::reset_fence() const {
    auto result = LogicalDevice::native().resetFences(
        1u,
        &_present_fences[_image_index]
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not reset present fences");
    }
}

// =============================================================================
void Swapchain::submit(const vk::CommandBuffer &command_buffer,
                       const CmdQueue &cmd_queue) const
{
    vk::Semaphore wait_sems[] = {
        _image_available_sems[_image_index]
    };

    vk::PipelineStageFlags wait_stage_masks[] {
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };

    vk::Semaphore signal_sems[] = {
        _draw_complete_sems[_image_index]
    };

    vk::SubmitInfo submit_info {
        .waitSemaphoreCount = static_cast<uint32_t>(std::size(wait_sems)),
        .pWaitSemaphores = wait_sems,
        .pWaitDstStageMask = wait_stage_masks,
        .commandBufferCount = 1u,
        .pCommandBuffers = &command_buffer,
        .signalSemaphoreCount =
            static_cast<uint32_t>(std::size(signal_sems)),
        .pSignalSemaphores = signal_sems,
    };

    // submit the graphics command buffer
    auto result = cmd_queue.native().submit(
        submit_info,
        _present_fences[_image_index]
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Could not submit command buffer to graphics queue.");
    }
}

// =============================================================================
vk::Result Swapchain::present() const {
    const auto &cmd_queue = LogicalDevice::cmd_queue().native();

    // notify the present buffer that we're going to wait for the current
    // frame to finsh/for the next vertical refresh
    vk::PresentInfoKHR present_info {
        .waitSemaphoreCount = 1u,
        .pWaitSemaphores = &_draw_complete_sems[_image_index],
        .swapchainCount = 1u,
        .pSwapchains = &_swapchain,
        .pImageIndices = &_image_index
    };

    // once more, do the thing and check to see if anything funky happened
    // along the way
    auto result = cmd_queue.presentKHR(present_info);
    if(result == vk::Result::eErrorOutOfDateKHR ||
        result == vk::Result::eSuboptimalKHR)
    {
        CONSOLE_WARN("presentKHR() returned '{}'", to_string(result));
    }

    return result;
}

// =============================================================================
void Swapchain::create() {
    _query_surface_capabilities();
    _query_surface_format();
    _query_surface_present_modes();
    _set_create_info();

    auto result = LogicalDevice::native().createSwapchainKHR(_create_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create swapchain");
    }
    _swapchain = result.value;
    CONSOLE_TRACE("Created swapchain for logical device");

    _get_images();
    _create_image_views();
    _create_synchronization();
}

// =============================================================================
void Swapchain::destroy() {
    for(auto &sem : _image_available_sems) {
        LogicalDevice::native().destroy(sem);
    }    
    for(auto &sem : _draw_complete_sems) {
        LogicalDevice::native().destroy(sem);
    }    
    for(auto &fence : _present_fences) {
        LogicalDevice::native().destroy(fence);
    }
    for(auto &image : _images) {
        ImageTools::destroy_view(image.view);
    }
    
    LogicalDevice::native().destroy(_swapchain);
}

// =============================================================================
void Swapchain::_query_surface_capabilities() {
    const auto &vk_pdev = PhysicalDevice::native();
    const auto &surface = TargetWindow::surface();
    const auto &result = vk_pdev.getSurfaceCapabilitiesKHR(surface);
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

// =============================================================================
void Swapchain::_query_surface_format() {
    const auto &vk_pdev = PhysicalDevice::native();
    const auto &surface = TargetWindow::surface();
    const auto &fmt_result  = vk_pdev.getSurfaceFormatsKHR(surface);
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
        if(format.format == vk::Format::eR8G8B8A8Unorm &&
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

// =============================================================================
void Swapchain::_query_surface_present_modes() {
    const auto &vk_pdev = PhysicalDevice::native();
    const auto &surface = TargetWindow::surface();
    const auto &mode_result = vk_pdev.getSurfacePresentModesKHR(surface);
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
    const auto &surface = TargetWindow::surface();
    // Now we're ready to fill out this struct
    _create_info = {
        .surface         = surface,
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

    // All is well and we don't have to worry about sharing this swapchain
    // between separate device queues.
    _create_info.imageSharingMode = vk::SharingMode::eExclusive;
    _create_info.queueFamilyIndexCount = 0;
    _create_info.pQueueFamilyIndices = nullptr;

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

// =============================================================================
void Swapchain::_get_images() {
    auto result = LogicalDevice::native().getSwapchainImagesKHR(_swapchain);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not get swapchain images");
    }

    if(result.value.size() != RenderConfig::swapchain_image_count) {
        CONSOLE_CRITICAL(
            "Swapchain returned {} images; {} requested",
            _images.size(), RenderConfig::swapchain_image_count
        );
    }
    
    _images.resize(RenderConfig::swapchain_image_count);
    for(uint32_t image_idx = 0u; image_idx < _images.size(); ++image_idx) {
        _images[image_idx].image = result.value[image_idx];
    }
}

// =============================================================================
void Swapchain::_create_image_views() {
    for(auto &image : _images) {
        ImageTools::create_view(
            image,
            vk::ImageViewType::e2D,
            _surface_format,
            vk::ImageAspectFlagBits::eColor
        );
    }
}

// =============================================================================
void Swapchain::_create_synchronization() {
    // Set aside the room for image-count-number of synchronization primitives
    _image_available_sems.resize(RenderConfig::swapchain_image_count);
    _draw_complete_sems.resize(RenderConfig::swapchain_image_count);
    _present_fences.resize(RenderConfig::swapchain_image_count);

    vk::SemaphoreCreateInfo sem_info { };
    vk::Result sync_result;

    // the semephores which will let us know when the swapchain has finished
    // whatever it was doing with one of the images
    for(auto &sem : _image_available_sems) {
        std::tie(sync_result, sem) =
            LogicalDevice::native().createSemaphore(sem_info);
        if(sync_result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Unable to create image available semaphore");
        }
    }

    // the semephores letting us know when a draw has completed to the back
    // buffer/image
    for(auto &sem : _draw_complete_sems) {
        std::tie(sync_result, sem) =
            LogicalDevice::native().createSemaphore(sem_info);
        if(sync_result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Unable to create draw complete semaphore");
        }
    }

    // Once there's a frame being written to the monitor and a frame on the
    // back buffer, the CPU needs to wait on the GPU before more frames can be
    // submitted.
    vk::FenceCreateInfo fence_info {
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    for(auto &fence : _present_fences) {
        std::tie(sync_result, fence) =
            LogicalDevice::native().createFence(fence_info);
        if(sync_result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Unable to create display fence");
        }
    }

    CONSOLE_TRACE("Created synchronization primitives");
}

// =============================================================================
Swapchain::Swapchain() :
    _create_info    { },
    _swapchain      { },
    _surface_format { vk::Format::eUndefined },
    _color_space    { vk::ColorSpaceKHR::eSrgbNonlinear },
    _image_index    { 0u },
    _offset         { 0, 0 },
    _extent         { 0u, 0u },
    _present_mode   { vk::PresentModeKHR::eImmediate }
{ }

} // namespace vkl