#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/swapchain/vkSurface.hpp"

#include "vklearnin/platform/TargetWindow.hpp"
#include "vklearnin/vulkan/vkInstance.hpp"
#include "vklearnin/vulkan/devices/vkPhysicalDevice.hpp"

namespace vkl {

// =============================================================================
bool vkSurface::create(TargetWindow const &target_window,
                       vkInstance const &instance,
                       Config const &config)
{
    if(_handle) {
        Log::error("Surface {} already exists.", _handle);
        return false;
    }

    if(!instance.native()) {
        Log::error("Cannot create surface with invalid instance.");
        return false;
    }

    _instance = instance.native();

    _enable_vsync = config.enable_vsync;

    // Populate the create info struct for our current platform
#ifdef VKL_LINUX
    vk::XlibSurfaceCreateInfoKHR const create_info {
        .pNext = nullptr,
        .flags = { },
        .dpy = target_window.display(),
        .window = target_window.native(),
    };

    auto const [ result, value ] = _instance.createXlibSurfaceKHR(create_info);
#elif VKL_WINDOWS
    vk::Win32SurfaceCreateInfoKHR const create_info {
        .pNext = nullptr,
        .flags = { },
        .hinstance = nullptr,
        .hwnd = target_window.native(),
    };

    auto const [ result, value ] = _instance.createWin32SurfaceKHR(create_info);
#endif // VKL platform

    if(result != vk::Result::eSuccess) {
        Log::error("Failed to create surface.");
        return false;
    }

    _handle = value;
    Log::trace("Created surface {}", _handle);

    return true;
}

// =============================================================================
bool vkSurface::destroy() {
    if(!_handle) {
        Log::error("Must create surface before calling destroy.");
        return false;
    }

    Log::trace("Destroying surface {}", _handle);
    _instance.destroy(_handle);
    _handle = nullptr;
    _instance = nullptr;

    return true;
}

// =============================================================================
void vkSurface::get_details(vkPhysicalDevice const &device) {
    _get_capabilities(device.native());
    _get_formats(device.native());
    _get_present_modes(device.native());
}

// =============================================================================
void vkSurface::_get_capabilities(vk::PhysicalDevice const &device) {
    auto const [ result, caps ] = device.getSurfaceCapabilitiesKHR(_handle);

    if(result != vk::Result::eSuccess) {
        Log::error("Failed to get surface {} capabilities: '{}'",
                  _handle, vk::to_string(result));
        return;
    }

    Log::trace(
        "\nSurface Capabilities:"
        "\n     Minimum Image Count: {}"
        "\n     Maximum Image Count: {}"
        "\n     Current Extent: {} x {}"
        "\n     Minimum Extent: {} x {}"
        "\n     Maximum Extent: {} x {}"
        "\n     Maximum Image Array Layers: {}",
        caps.minImageCount,
        caps.maxImageCount,
        caps.currentExtent.width,
        caps.currentExtent.height,
        caps.minImageExtent.width,
        caps.minImageExtent.height,
        caps.maxImageExtent.width,
        caps.maxImageExtent.height,
        caps.maxImageArrayLayers
    );

    _min_image_count = caps.minImageCount;
    _max_image_count = caps.maxImageCount;

    if(_max_image_count == 0u) {
        _max_image_count = std::numeric_limits<uint32_t>::max();
    }

    if(_min_image_count > _max_image_count) {
        Log::error(
            "Surface {} minimum image count {} exceeds maximum image count {}",
            _handle,
            _min_image_count,
            _max_image_count
        );

        _min_image_count = 0u;
        _max_image_count = 0u;

        return;
    }

    // We intend to draw to the whole surface
    _extent = caps.currentExtent;

    if(_extent.width < caps.minImageExtent.width) {
        Log::warn("Surface width {} capped to minimum {}",
                  _extent.width,
                  caps.minImageExtent.width);

        _extent.width = caps.minImageExtent.width;
    }
    else if(_extent.width > caps.maxImageExtent.width) {
        Log::warn("Surface width {} capped to maximum {}",
                  _extent.width,
                  caps.minImageExtent.width);

        _extent.width = caps.maxImageExtent.width;
    }

    if(_extent.height < caps.minImageExtent.height) {
        Log::warn("Surface height {} capped to minimum {}",
                  _extent.height,
                  caps.minImageExtent.height);

        _extent.height = caps.minImageExtent.height;
    }
    else if(_extent.height > caps.maxImageExtent.height) {
        Log::warn("Surface height {} capped to maximum {}",
                  _extent.height,
                  caps.minImageExtent.height);

        _extent.height = caps.maxImageExtent.height;
    }

    // Update the aspect ratio
    _aspect_ratio = static_cast<float>(_extent.width) /
                    static_cast<float>(_extent.height);
}

// =============================================================================
void vkSurface::_get_formats(vk::PhysicalDevice const &device) {
    auto const [ result, formats ] = device.getSurfaceFormatsKHR(_handle);

    if(result != vk::Result::eSuccess) {
        Log::error("Failed to get surface {} formats: '{}'",
                  _handle, vk::to_string(result));
        return;
    }

    Log::trace("Found {} surface formats.", formats.size());

    // These format details were chosen to produce the most intuitive and/or
    // predictable results on the average desktop disaply
    auto const desired_format = vk::Format::eB8G8R8A8Unorm;
    auto const deisred_space = vk::ColorSpaceKHR::eSrgbNonlinear;

    bool found_desired = false;

    for(auto const& format : formats) {
        if(format.format == desired_format &&
           format.colorSpace == deisred_space)
        {
            _format = format;
            found_desired = true;
        }

        Log::trace("    {} / {}",
                  vk::to_string(format.format),
                  vk::to_string(format.colorSpace));
    }

    if(!found_desired) {
        // In the event that our desired combination isn't found, just go with
        // whatever the implementation has as its first result
        _format = formats[0];

        Log::warn("Could not find desired swapchain surface format/color space "
                 "of {} / {}. Defaulting instead to {} / {}.",
                 vk::to_string(desired_format),
                 vk::to_string(deisred_space),
                 vk::to_string(_format.format),
                 vk::to_string(_format.colorSpace));
    }
}

// =============================================================================
void vkSurface::_get_present_modes(vk::PhysicalDevice const &device) {
    auto const [ result, modes ] = device.getSurfacePresentModesKHR(_handle);

    if(result != vk::Result::eSuccess) {
        Log::error("Failed to get surface {} present modes: '{}'",
                  _handle, vk::to_string(result));
        return;
    }

    Log::trace("Found {} present modes.", modes.size());

    // This is the order of preference for present modes:
    bool has_fifo_relaxed = false;  // V-Sync on, but with some latency tweaks
    bool has_fifo         = false;  // Strict V-Sync
    bool has_immediate    = false;  // V-Sync off; draw as fast as possible

    for(auto const mode : modes) {
        Log::trace("    {}", vk::to_string(mode));
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

    // Use a FIFO variant if they're available and V-Sync has been chosen by
    // the user
    if(has_fifo_relaxed && _enable_vsync) {
        _present_mode = vk::PresentModeKHR::eFifoRelaxed;
    }
    else if(has_fifo && _enable_vsync) {
        _present_mode = vk::PresentModeKHR::eFifo;
    }
    else if(_enable_vsync) {
        Log::warn("V-Sync requested but the available present modes don't "
                 "support it.");
    }
    else if(has_immediate) {
        _present_mode = vk::PresentModeKHR::eImmediate;
    }
    else {
        Log::critical("Neither immediate nor FIFO presentation modes are "
                     "supported.");
    }
}

} // namespace vkl