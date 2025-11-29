#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/swapchain/vkSwapchain.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/devices/vkQueue.hpp"
#include "vklearnin/vulkan/swapchain/vkSurface.hpp"
#include "vklearnin/vulkan/swapchain/vkFrameSync.hpp"
#include "vklearnin/vulkan/resources/vkImage.hpp"
#include "vklearnin/vulkan/resources/vkImageView.hpp"

namespace vkl {

// =============================================================================
bool vkSwapchain::create(vkDevice const &device, vkSurface const &surface,
                         uint32_t const min_image_offset) {
    if(_handle) {
        Log::error("Swapchain {} already exists", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create swapchain with invalid device.");
        return false;
    }

    if(!surface.native()) {
        Log::error("Cannot create swapchain with invalid surface.");
        return false;
    }

    _device = &device;
    _surface = &surface;

    _image_count = _surface->min_image_count() + min_image_offset;

    if(_image_count > _surface->max_image_count()) {
        Log::error("Requested swapchain image count {} exceeds maximum surface "
                   "image count {}.",
                   _image_count,
                   _surface->max_image_count());

        _device = nullptr;
        _surface = nullptr;
        _image_count = 0u;

        return false;
    }

    _images.resize(_image_count);
    _image_views.resize(_image_count);

    _populate_create_info();

    // Finally, create the swapchain
    _handle = _device->native().createSwapchainKHR(_create_info);
    Log::trace(
        "Created swapchain {} with {} frames in flight",
        _handle,
        _image_count
    );

    // Now that we've got the swapchain itself, we'll need its images
    _get_images();

    return true;
}

// =============================================================================
bool vkSwapchain::destroy() {
    if(!_handle) {
        Log::error("Must create swapchain before calling destroy.");
        return false;
    }

    _images.clear();

    for(auto &view : _image_views) {
        view.destroy();
    }
    _image_views.clear();

    Log::trace("Destroying swapchain {}", _handle);
    _device->native().destroy(_handle);
    _handle = nullptr;
    _device = nullptr;

    return true;
}

// =============================================================================
uint32_t vkSwapchain::acquire_next_image(vk::Semaphore const &signal_sem) const {
    // Wait for no more than one second
    using namespace std::chrono_literals;
    static auto const wait_period =
        std::chrono::duration_cast<std::chrono::nanoseconds>(1.0s).count();

    auto const [ result, next_frame ] = _device->native().acquireNextImageKHR(
        _handle,
        wait_period,
        signal_sem
    );

    if(result != vk::Result::eSuccess) {
        Log::warn("Failed to acquire next swapchain image: '{}'",
                  vk::to_string(result));

        return std::numeric_limits<uint32_t>::max();
    }

    return next_frame;
}

// =============================================================================
void vkSwapchain::_populate_create_info() {
    _create_info = {
        .surface         = _surface->native(),
        .minImageCount   = static_cast<uint32_t>(_images.size()),
        .imageFormat     = _surface->format().format,
        .imageColorSpace = _surface->format().colorSpace,
        .imageExtent     = _surface->extent(),

        // Image array layers will always be one, except in the case of a
        // device with multiple displays interested in the same swapchain,
        // like a VR headset
        .imageArrayLayers = 1u,

        // Marking the images in this swapchain as color attachments means they
        // can be used to draw into
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,

        // We don't need to worry about sharing images between queues because
        // vkPhysicalDevice insisted on having a queue family that could accept
        // both graphics and present commands
        .imageSharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,

        // Transforms are what you'd expect: the image can be flipped, rotated,
        // etc. Leaving it alone gets us what we want for now.
        .preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,

        // No support for transparency or color blending for now
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,

        // Store the selected present mode
        .presentMode = _surface->present_mode(),

        // Setting clipped to true allows Vulkan to ignore pixels on the render
        // surface that can't be seen. For example, moving another window in
        // front of the render surface
        .clipped = vk::True,

        // There are situations during which you'll want to recreate the
        // swapchain, and providing an existing example can speed up the
        // process.
        .oldSwapchain = nullptr,
    };

    Log::trace("\nSwapchain Create Info:"
               "\n    Extent:       {}x{}"
               "\n    Image Count:  {}"
               "\n    Format:       {}"
               "\n    Color Space:  {}"
               "\n    Present Mode: {}",
               _create_info.imageExtent.width, _create_info.imageExtent.height,
               _create_info.minImageCount,
               vk::to_string(_create_info.imageFormat),
               vk::to_string(_create_info.imageColorSpace),
               vk::to_string(_create_info.presentMode));
}

// =============================================================================
void vkSwapchain::_get_images() {
    auto const swapchain_images =
        _device->native().getSwapchainImagesKHR(_handle);

    if(swapchain_images.size() != _images.size()) {
        Log::error("Swapchain provided {} images; expected {}",
                  swapchain_images.size(), _image_count);
        return;
    }

    Log::trace("Acquired {} swapchain images", swapchain_images.size());

    for(uint32_t i = 0u; i < _images.size(); ++i) {
        std::string debug_name { std::format("Swapchain image {}", i).c_str() };

        _images[i].create(
            swapchain_images[i],
            _surface->format().format,
            vk::Extent3D {
                .width = _surface->extent().width,
                .height = _surface->extent().height,
            },
            *_device,
            debug_name
        );

        _image_views[i].create(
            vkImageView::Details {
                .image = _images[i].native(),
                .format = _images[i].format(),
                .type = vk::ImageViewType::e2D,
                .aspect_flags = vk::ImageAspectFlagBits::eColor
            },
            *_device
        );
    }
}

} // namespace vkl
