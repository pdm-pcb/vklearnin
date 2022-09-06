#include "vklearnin/common.hpp"
#include "vklearnin/Swapchain.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/CommandStructures/CommandQueues.hpp"

//==============================================================================
void Swapchain::init_color_format() {
    CONSOLE_INFO("");

    // query and populate the list of available color spaces/image formats
    auto formats = _instance.physical_device().getSurfaceFormatsKHR(_surface);
    if(formats.size() == 0) {
        CONSOLE_CRITICAL("Found zero surface color formats");
    }
    CONSOLE_TRACE("Found {} color formats", formats.size());

    // presumably, we want 32-bit SRGB
    for(const auto format : formats) {
        if(format.format == vk::Format::eB8G8R8A8Srgb &&
           format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            _color_format = format.format;
            _color_space  = format.colorSpace;
        }

        CONSOLE_TRACE(
            "Color format: {} / {}",
            to_string(format.format),
            to_string(format.colorSpace)
        );
    }

    // otherwise... just take the first available format?
    if(_color_format == vk::Format::eUndefined)
    {
        _color_format = formats[0].format;
        _color_space = formats[0].colorSpace;

        CONSOLE_WARN("Assigned default color format: {} / {}",
                      to_string(_color_format), to_string(_color_space));
    }
    else {
        CONSOLE_TRACE("Assigned color format: {} / {}",
                      to_string(_color_format), to_string(_color_space));
    }
}

//==============================================================================
void Swapchain::init_present_modes() {
    CONSOLE_INFO("");

    // query and populate the list of presentation modes
    auto modes =
        _instance.physical_device().getSurfacePresentModesKHR(_surface);
    
    if(modes.size() == 0) {
        CONSOLE_CRITICAL("Found zero surface present modes.");
    }
    CONSOLE_TRACE("Found {} surface present modes", modes.size());

    // iterate available modes, and choose FIFO/V-Sync by default, if it's
    // available
    for(const auto mode : modes) {
        CONSOLE_TRACE("Presentation mode: {}", to_string(mode));
        if(mode == vk::PresentModeKHR::eFifo) {
            _present_mode = mode;
        }
    }

    // default to immediate/unsynchronized presentations if there's nothing
    // else
    // if(_present_mode != vk::PresentModeKHR::eFifo) {
        _present_mode = vk::PresentModeKHR::eImmediate;
        CONSOLE_WARN("Assigning default immedate presentation mode.");
    // }
    // else {
    //     CONSOLE_TRACE("FIFO present mode chosen");
    // }
}

//==============================================================================
void Swapchain::init_extent(const vk::Extent2D &extent) {
    CONSOLE_INFO("");

    auto surface_capabilities =
        _instance.physical_device().getSurfaceCapabilitiesKHR(_surface);

    if(surface_capabilities.maxImageCount == 0) {
        CONSOLE_CRITICAL("Physical device capabilities reported zero images.");
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

    // Don't exceed the maximum image count, however. Zero is a special maximum
    // indicating unlimiated images
    if(surface_capabilities.maxImageCount != 0u &&
       FRAME_OVERLAP >= surface_capabilities.maxImageCount)
    {
        CONSOLE_CRITICAL(
            "Configured image count {} exceeds surface max of {}",
            FRAME_OVERLAP,
            surface_capabilities.maxImageCount
        );
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
        _extent = extent;
    }
    else {
        _extent = surface_capabilities.currentExtent;
    }

    _transform = surface_capabilities.currentTransform;
    CONSOLE_TRACE("Surface current transform: {}", to_string(_transform));

    _aspect_ratio = static_cast<float>(_extent.width) / _extent.height;
}

//==============================================================================
void Swapchain::init_swapchain(const CommandQueues &queues) {
    CONSOLE_INFO("");

    // now we've got everything we need to actually create the swapchain
    vk::SwapchainCreateInfoKHR swapchain_info {
        .surface          = _surface,
        .minImageCount    = FRAME_OVERLAP,
        .imageFormat      = _color_format,
        .imageColorSpace  = _color_space,
        .imageExtent      = _extent,
        .imageArrayLayers = _image_array_layers,
        .preTransform     = _transform,
        .presentMode      = _present_mode,
    };

    uint32_t indices[] = {
        queues.graphics_index(),
        queues.present_index()
    };

    // the queue families should be the same, but if they're not, then the
    // difference must be explicitly noted and a concurrent image sharing mode
    // used
    if(queues.graphics_index() != queues.present_index())
    {
        swapchain_info.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.pQueueFamilyIndices = indices;
        CONSOLE_TRACE("Swapchain being created with concurrent sharing mode.");
    }
    else {
        swapchain_info.imageSharingMode = vk::SharingMode::eExclusive;
        swapchain_info.queueFamilyIndexCount = 0;
        swapchain_info.pQueueFamilyIndices = nullptr;
        CONSOLE_TRACE("Swapchain being created with exclusive sharing mode.");
    }
    
    // color attachment denotes immediate rendering, while setting composite
    // alpha to opaque simply disables any blending
    swapchain_info.imageUsage     = vk::ImageUsageFlagBits::eColorAttachment;
    swapchain_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

    // don't render pixels on the surface which are obscured by eg, another
    // window
    swapchain_info.clipped = VK_TRUE;

    // since this is the first time we're creating a swapchain for the
    // application, there's no previous handle
    swapchain_info.oldSwapchain = nullptr;

    _swapchain = _instance.logical_device().createSwapchainKHR(swapchain_info);
}

//==============================================================================
void Swapchain::init_swapchain_images() {
    CONSOLE_INFO("");

    _images = _instance.logical_device().getSwapchainImagesKHR(_swapchain);
    if(_images.size() < FRAME_OVERLAP) {
        CONSOLE_ERROR(
            "Swapchain only supports {} images, while {} requested",
            _images.size(), FRAME_OVERLAP
        );
    }
}

//==============================================================================
void Swapchain::init_image_views() {
    CONSOLE_INFO("");

    _image_views.resize(FRAME_OVERLAP);    
    for(size_t image = 0; image < FRAME_OVERLAP; ++image) {
        _image_views[image] = ImageTools::init_view(
            _images[image],
            _color_format,
            1u,
            vk::ImageAspectFlagBits::eColor,
            _instance.logical_device()
        );
    }
}

//==============================================================================
void Swapchain::destroy() {
    CONSOLE_INFO("");

    for(auto &view : _image_views) {
        _instance.logical_device().destroy(view);
    }
    
    _instance.logical_device().destroy(_swapchain);
}

//==============================================================================
void Swapchain::create(const vk::Extent2D &extent, const CommandQueues &queues,
                       const vk::SurfaceKHR &surface)
{
    CONSOLE_INFO("");

    _surface = surface;

    init_color_format();
    init_present_modes();
    init_extent(extent);
    init_swapchain(queues);
    init_swapchain_images();
    init_image_views();
}

//==============================================================================
Swapchain::Swapchain(const Instance &instance, vk::SurfaceKHR &surface) :
    _color_format       { vk::Format::eUndefined },
    _color_space        { vk::ColorSpaceKHR::eSrgbNonlinear },
    _image_array_layers { 1u }, // layers > 1 are for stereoscopic 3D
    _offset             { 0, 0 },
    _extent             { UI32MAX, UI32MAX },
    _aspect_ratio       { 0.0f },
    _present_mode       { vk::PresentModeKHR::eImmediate },
    _transform          { vk::SurfaceTransformFlagBitsKHR::eIdentity },
    _swapchain          { nullptr  },
    _old_swapchain      { nullptr  },
    _instance           { instance },
    _surface            { surface  }
{
    CONSOLE_INFO("");
}

Swapchain::~Swapchain() {
    CONSOLE_INFO("");

    destroy();
}