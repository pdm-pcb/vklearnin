#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/swapchain/Swapchain.hpp"

#include "vklearnin/system/devices/PhysicalDevice.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/system/devices/CmdQueue.hpp"
#include "vklearnin/system/window/TargetWindow.hpp"
#include "vklearnin/resources/images/ImageTools.hpp"
#include "vklearnin/resources/images/Texture2D.hpp"

namespace vkl {

vk::Format         Swapchain::_image_format = vk::Format::eUndefined;
vk::ColorSpaceKHR  Swapchain::_color_space  = vk::ColorSpaceKHR::eSrgbNonlinear;
vk::Extent2D       Swapchain::_extent { 0u, 0u };
vk::Offset2D       Swapchain::_offset { 0, 0 };
vk::PresentModeKHR Swapchain::_present_mode = vk::PresentModeKHR::eImmediate;

vk::SwapchainCreateInfoKHR Swapchain::_create_info { };
vk::SwapchainKHR           Swapchain::_swapchain   { };

std::vector<ImageObject>          Swapchain::_images;
std::vector<Swapchain::ImageSync> Swapchain::_image_sync;

uint32_t Swapchain::_draw_index    = 0u;
uint32_t Swapchain::_present_index = 0u;

// =============================================================================
void Swapchain::next_image() {
    auto sync = _image_sync[_present_index];

    // Wait for the GPU to signal on the present fence, indicating there's an
    // image for the CPU to schedule work for
    auto result = LogicalDevice::native().waitForFences(
        1u,                // fence count
        &sync.queue_fence, // which fence to wait on
        VK_TRUE,           // require all fences to signal?
        std::numeric_limits<uint64_t>::max() // block effectively forever
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("waitForFences() returned '{}'", to_string(result));
    }

    // Since the fence has been signaled, we can now ask the swapchain which
    // image it'd like us to work with
    result = LogicalDevice::native().acquireNextImageKHR(
        { _swapchain },                      // acquire from yourself
        std::numeric_limits<int64_t>::max(), // block effectively forever
        sync.present_complete,               // which sem to signal
        nullptr,                             // which fence to signal (none)
        &_draw_index                         // resulting image index
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_WARN("acquireNextImageKHR() returned '{}'", to_string(result));
    }
}

// =============================================================================
void Swapchain::reset_fence() {
    auto result = LogicalDevice::native().resetFences(
        1u,
        &_image_sync[_present_index].queue_fence
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("resetFences() returned '{}'", to_string(result));
    }
}

// =============================================================================
void Swapchain::submit(const std::vector<vk::CommandBuffer> &buffers) {
    // Internally, the command queue needs to know when to begin. In this case,
    // waiting on a completed presentation means there's an image available for
    // writing to.
    vk::Semaphore wait_sems[] = {
        _image_sync[_present_index].present_complete
    };

    // The above semaphore(s) will wait at this stage. In keeping with the idea
    // that these are all graphics commands being submitted, waiting at the
    // very bottom of the pipeline is the right choice.
    vk::PipelineStageFlags wait_dst_stage[] {
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };

    // Some semaphores may also want to know when this particular batch of
    // commands is complete, so they get listed here.
    vk::Semaphore signal_sems[] = {
        _image_sync[_draw_index].draw_complete
    };

    const vk::SubmitInfo submit_info {
        .waitSemaphoreCount = static_cast<uint32_t>(std::size(wait_sems)),
        .pWaitSemaphores = wait_sems,
        .pWaitDstStageMask = wait_dst_stage,
        .commandBufferCount = static_cast<uint32_t>(buffers.size()),
        .pCommandBuffers = buffers.data(),
        .signalSemaphoreCount =
            static_cast<uint32_t>(std::size(signal_sems)),
        .pSignalSemaphores = signal_sems,
    };

    auto result = LogicalDevice::cmd_queue().native().submit(
        submit_info,
        _image_sync[_present_index].queue_fence
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR(
            "Could not submit command buffers: '{}'",
            to_string(result)
        );
    }
}

// =============================================================================
void Swapchain::present() {
    // Now that the image being drawn is ready to be presented, these indices
    // will overlap for a touch
    _present_index = _draw_index;

    const vk::PresentInfoKHR present_info {
        // Do make sure the previously submitted work is acctually complete
        .waitSemaphoreCount = 1u,
        .pWaitSemaphores = &_image_sync[_draw_index].draw_complete,

        // With only one surface in this setup, there'll only ever be one
        // swapchain
        .swapchainCount = 1u,
        .pSwapchains = &_swapchain,

        // Again, at the moment both image indices are the same
        .pImageIndices = &_present_index
    };

    auto result = LogicalDevice::cmd_queue().native().presentKHR(present_info);
    if(result == vk::Result::eErrorOutOfDateKHR ||
       result == vk::Result::eSuboptimalKHR)
    {
        CONSOLE_WARN("presentKHR() returned '{}'", to_string(result));
    }
}

// =============================================================================
void Swapchain::create() {
    _query_surface_capabilities();
    _query_surface_format();
    _query_surface_present_modes();
    _populate_create_info();

    auto result = LogicalDevice::native().createSwapchainKHR(_create_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to create swapchain: '{}'", to_string(result.result)
        );
    }
    _swapchain = result.value;
    CONSOLE_TRACE("Created swapchain for logical device");

    _get_images();
    _create_image_views();
    _create_sync_primitives();
}

// =============================================================================
void Swapchain::destroy() {
    for(auto &sync : _image_sync) {
        CONSOLE_TRACE(
            "Destroying swapchain sync primitives:"
            "\n\tavailable semaphore {:#x}"
            "\n\tdraw      semaphore {:#x}"
            "\n\tpresent   fence     {:#x}",
            reinterpret_cast<uint64_t>(VkSemaphore(sync.present_complete)),
            reinterpret_cast<uint64_t>(VkSemaphore(sync.draw_complete)),
            reinterpret_cast<uint64_t>(VkFence(sync.queue_fence))
        );
        LogicalDevice::native().destroySemaphore(sync.present_complete);
        LogicalDevice::native().destroySemaphore(sync.draw_complete);
        LogicalDevice::native().destroyFence(sync.queue_fence);
    }
    for(auto &image : _images) {
        ImageTools::destroy_view(image);
    }
    
    LogicalDevice::native().destroy(_swapchain);
}

// =============================================================================
void Swapchain::_query_surface_capabilities() {
    auto const& gpu = PhysicalDevice::native();
    auto const& surface = TargetWindow::surface();
    auto const& result = gpu.getSurfaceCapabilitiesKHR(surface);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not get surface capabilities: '{}'",
            to_string(result.result)
        );
    }
    auto const& capabilities = result.value;

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

    // Provided image_count has already been used to set some array sizes (in
    // LogicalDevice, for example) it's become a hard requirement of the
    // surface itself
    if(RenderConfig::image_count > capabilities.maxImageCount ||
       RenderConfig::image_count < capabilities.minImageCount)
    {
        CONSOLE_CRITICAL(
            "{} swapchain images requested, but surface allows a minimum of {} "
            "and a maximum of {} images",
            RenderConfig::image_count,
            capabilities.minImageCount,
            capabilities.maxImageCount
        );
    }
}

// =============================================================================
void Swapchain::_query_surface_format() {
    auto const& gpu = PhysicalDevice::native();
    auto const& surface = TargetWindow::surface();
    auto const& result  = gpu.getSurfaceFormatsKHR(surface);
    if(result.result != vk::Result::eSuccess || result.value.empty()) {
        CONSOLE_CRITICAL(
            "Could not get surface formats.: '{}'",
            to_string(result.result)
        );
    }
    auto const& formats = result.value;
    CONSOLE_TRACE("Found {} surface formats.", formats.size());

    // First, default to the image format details of the first listed - these
    // are only used if we can't find the desired combo in the for loop below.
    _image_format = formats[0].format;
    _color_space    = formats[0].colorSpace;

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
    auto const& result = gpu.getSurfacePresentModesKHR(surface);
    if(result.result != vk::Result::eSuccess || result.value.empty()) {
        CONSOLE_CRITICAL(
            "Could not get surface present modes: '{}'",
            to_string(result.result)
        );
    }

    auto const& modes = result.value;
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
void Swapchain::_populate_create_info() {
    _create_info = {
        .surface         = TargetWindow::surface(),
        .minImageCount   = RenderConfig::image_count,
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
        RenderConfig::image_count,
        to_string(_image_format),
        to_string(_color_space),
        to_string(_present_mode)
    );
}

// =============================================================================
void Swapchain::_get_images() {
    auto result = LogicalDevice::native().getSwapchainImagesKHR(_swapchain);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not get swapchain images: '{}'",
            to_string(result.result)
        );
    }

    if(result.value.size() != RenderConfig::image_count) {
        CONSOLE_CRITICAL(
            "Swapchain returned {} images; {} requested",
            _images.size(), RenderConfig::image_count
        );
    }
    
    _images.resize(RenderConfig::image_count);
    for(uint32_t image_idx = 0u; image_idx < _images.size(); ++image_idx) {
        _images[image_idx] = {
            .handle = result.value[image_idx],
            .format = _image_format,
            .layout =  vk::ImageLayout::eUndefined,
        };
    }
}

// =============================================================================
void Swapchain::_create_image_views() {
    for(auto &image : _images) {
        ImageTools::create_view(
            image,
            vk::ImageViewType::e2D,
            vk::ImageAspectFlagBits::eColor
        );
    }
}

// =============================================================================
void Swapchain::_create_sync_primitives() {
    // Set aside the room for image-count-number of synchronization primitives
    _image_sync.resize(RenderConfig::image_count);

    const vk::SemaphoreCreateInfo sem_info { };
    vk::Result result;

    for(auto &sync : _image_sync) {
        // First, the semephores which will let us know when the swapchain has
        // finished presenting one of the images
        std::tie(result, sync.present_complete) =
            LogicalDevice::native().createSemaphore(sem_info);
        if(result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL(
                "Unable to create present semaphore: '{}'",
                to_string(result)
        );
            return;
        }

        // Next, the semephores letting us know when a draw has completed to the
        // back buffer/image
        std::tie(result, sync.draw_complete) =
            LogicalDevice::native().createSemaphore(sem_info);
        if(result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL(
                "Unable to swapchain draw semaphore: '{}'",
                to_string(result)
        );
            return;
        }

        // Once there's a frame being written to the monitor and a frame on the
        // back buffer, the CPU needs to wait on the GPU before more frames can
        // be submitted.
        const vk::FenceCreateInfo fence_info {
            .flags = vk::FenceCreateFlagBits::eSignaled
        };
        std::tie(result, sync.queue_fence) =
            LogicalDevice::native().createFence(fence_info);
        if(result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL(
                "Unable to create swapchain queue fence: '{}'",
                to_string(result)
        );
            return;
        }

        CONSOLE_TRACE(
            "Created swapchain sync primitives:"
            "\n\tavailable semaphore {:#x}"
            "\n\tdraw      semaphore {:#x}"
            "\n\tpresent   fence     {:#x}",
            reinterpret_cast<uint64_t>(VkSemaphore(sync.present_complete)),
            reinterpret_cast<uint64_t>(VkSemaphore(sync.draw_complete)),
            reinterpret_cast<uint64_t>(VkFence(sync.queue_fence))
        );
    }
}

} // namespace vkl