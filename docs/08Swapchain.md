## Organizing the Images
A [swapchain](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSwapchainKHR.html) represents, among other things, a series of images in Vulkan. For a traditional double-buffered setup, the length of the chain is two. While one image is being built by the graphics card (being acted upon by the graphics command queue), the other is being drawn by the display (being acted upon by the present command queue). Rinse and repeat, with fewer issues this time. We've still effectively got two separate computers trying to work together, so they'll still need help staying synchronized.

Access to each swapchain image will be protected by a pair of [semaphores](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSemaphore.html). There will be one semaphore which will signal when the graphics queue has completed writing to the image in question, meaning the image is ready to be presented. There'll be another semaphore which will signal when the present queue has handed the image in question over to the display, meaning the image is ready to be drawn to.

## Swapchain
Moving on to the class itself, we'll start with barely any functionality exposed just to get things up and running.

```cpp
#ifndef VKLEARNIN_ENGINE_SWAPCHAIN_HPP
#define VKLEARNIN_ENGINE_SWAPCHAIN_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class VKQueue;
class Image2D;

class Swapchain final {
public:
    void create();
    void destroy();

    Swapchain();
    ~Swapchain();

    Swapchain(Swapchain &&) = delete;
    Swapchain(const Swapchain &) = delete;

    Swapchain & operator=(Swapchain &&) = delete;
    Swapchain & operator=(const Swapchain &) = delete;

private:
    vk::Format         _surface_format;
    vk::ColorSpaceKHR  _color_space;
    vk::Extent2D       _extent;
    vk::PresentModeKHR _present_mode;
    
    vk::SwapchainCreateInfoKHR _create_info;
    vk::SwapchainKHR _swapchain;

    struct ImageSync {
        vk::Semaphore available;
        vk::Semaphore draw_complete;
    };

    std::vector<Image2D *> _images;
    std::vector<ImageSync> _image_sync;

    void _query_surface_capabilities();
    void _query_surface_format();
    void _query_surface_present_modes();
    void _populate_create_info();
    void _get_images();
    void _create_image_views();
    void _create_synchronization();
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_SWAPCHAIN_HPP
```

There are a few new ideas here, namely `vk::ColorSpaceKHR` and `vk::PresentModeKHR`. [Color spaces](https://en.wikipedia.org/wiki/Color_space) are an interesting topic all their own, and for our rudimentary purposes they'll just inform what a given integer (or float) value for red, green, and blue will actually look like when displayed on screen. If you're curious, [here's](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkColorSpaceKHR.html) what the spec has to say on color spaces.

The swapchain images we're interested in creating will have a format of `R8G8B8A8Unorm` and will be in the color space `SrgbNonlinear`. This format means the red, green, blue, and alpha (transparency) channels of the image will all get eight bits of unsigned, normalized floating point data to describe themselves. The color space is chosen to match the default of most desktop and laptop displays.

Vulkan's concept of a [present mode](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPresentModeKHR.html) describes exactly how the API manages the asynchronous relationship between the graphics queue and the present queue. There are several modes to choose from. I'll focus on three modes.

Immediate mode allows the GPU to crank out completed images as fast as it possibly can; the display must simply do its best to keep up. FIFO mode requires the display to present one image at a time, respecting the order of submission. In this case, the GPU will sometimes be waiting on the display to complete presentation before it can begin drawing a new frame.

FIFO is the only mode the Vulkan spec requires, but there's a slight variation on it that we'll use if it's supported called FIFO Relaxed. This mode improves the coordination between the GPU and the display a little, making flickering less likely when the GPU falls behind.

Returning to the code, `Swapchain`'s constructor and destructor are fairly pedestrian:

```cpp
Swapchain::Swapchain() :
    _create_info         { },
    _swapchain           { },
    _surface_format      { vk::Format::eUndefined },
    _color_space         { vk::ColorSpaceKHR::eSrgbNonlinear },
    _extent              { 0u, 0u },
    _present_mode        { vk::PresentModeKHR::eImmediate }
{ }

Swapchain::~Swapchain() {
    for(auto image : _images) {
        delete image;
    }
}
```

`Swapchain::create()` is a convenience wrapper for the actual initialization steps, and integrates the first of our image code from above.

```cpp
void Swapchain::create() {
    _query_surface_capabilities();
    _query_surface_format();
    _query_surface_present_modes();
    _populate_create_info();

    auto result = VKLogicalDevice::native().createSwapchainKHR(_create_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create swapchain");
    }
    _swapchain = result.value;
    CONSOLE_TRACE("Created swapchain for logical device");

    _get_images();
    _create_image_views();
    _create_synchronization();
}
```

The first two functions request details from the `TargetWindow`'s render surface. As that surface is where the swapchain images will wind up being displayed, their properties should all match.

```cpp
// =============================================================================
void Swapchain::_query_surface_capabilities() {
    const auto &gpu = VKPhysicalDevice::native();
    const auto &surface = TargetWindow::surface();
    const auto &result = gpu.getSurfaceCapabilitiesKHR(surface);
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

    // Provided image_count has already been used to set some array sizes (in
    // VKLogicalDevice, for example) it's become a hard requirement of the
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
    const auto &gpu = VKPhysicalDevice::native();
    const auto &surface = TargetWindow::surface();
    const auto &fmt_result  = gpu.getSurfaceFormatsKHR(surface);
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
```

The final detail that the render surface will provide is the supported present mode. If `RenderConfig::vsync_on` is true, we'll prefer FIFO Relaxed, then FIFO, then Immediate, in that order.

```cpp
void Swapchain::_query_surface_present_modes() {
    const auto &gpu = VKPhysicalDevice::native();
    const auto &surface = TargetWindow::surface();
    const auto &mode_result = gpu.getSurfacePresentModesKHR(surface);
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
```

And the last step before attempting to create the swapchain will be stuffing all of that information into the swapchain's create info structure.

```cpp
void Swapchain::_populate_create_info() {
    _create_info = {
        .surface         = TargetWindow::surface(),
        .minImageCount   = RenderConfig::image_count,
        .imageFormat     = _surface_format,
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
        .clipped = true,

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
        to_string(_surface_format),
        to_string(_color_space),
        to_string(_present_mode)
    );
}
```

If you recall from `Swapchain::create()`, the next step is simply asking our logical device to create the swapchain for us. Once that's been completed, we can request the configured images from it and create image views so we can work with them.

```cpp
// =============================================================================
void Swapchain::_get_images() {
    auto result = VKLogicalDevice::native().getSwapchainImagesKHR(_swapchain);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not get swapchain images");
    }

    if(result.value.size() != RenderConfig::image_count) {
        CONSOLE_CRITICAL(
            "Swapchain returned {} images; {} requested",
            _images.size(), RenderConfig::image_count
        );
    }
    
    _images.resize(RenderConfig::image_count);
    for(uint32_t image_idx = 0u; image_idx < _images.size(); ++image_idx) {
        _images[image_idx] = new Image2D;
        _images[image_idx]->init(
            result.value[image_idx],
            _surface_format,
            vk::ImageLayout::eUndefined,
            _extent
        );
    }
}

// =============================================================================
void Swapchain::_create_image_views() {
    for(auto &image : _images) {
        image->create_view(vk::ImageAspectFlagBits::eColor);
    }
}
```

The last step in setting up the swapchain is creating the synchronization primitives required for double-buffering to function.

```cpp
void Swapchain::_create_synchronization() {
    // Set aside the room for image-count-number of synchronization primitives
    _image_sync.resize(RenderConfig::image_count);

    vk::SemaphoreCreateInfo sem_info { };
    vk::Result sync_result;

    for(auto &sync : _image_sync) {
        // First, the semephores which will let us know when the swapchain has
        // finished presenting one of the images
        std::tie(sync_result, sync.available) =
            VKLogicalDevice::native().createSemaphore(sem_info);
        if(sync_result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Unable to create image available semaphore");
        }

        // Next, the semephores letting us know when a draw has completed to the
        // back buffer/image
        std::tie(sync_result, sync.draw_complete) =
            VKLogicalDevice::native().createSemaphore(sem_info);
        if(sync_result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Unable to create draw complete semaphore");
        }

        CONSOLE_TRACE(
            "Created swapchain image semaphores {:#x} and {:#x}",
            reinterpret_cast<uint64_t>(VkSemaphore(sync.available)),
            reinterpret_cast<uint64_t>(VkSemaphore(sync.draw_complete))
        );
    }
}
```

Finally, `Swapchain::destroy()` will happily undo all our hard work.

```cpp
void Swapchain::destroy() {
    for(auto &sync : _image_sync) {
        CONSOLE_TRACE(
            "Destroying swapchain image semaphores {:#x} and {:#x}",
            reinterpret_cast<uint64_t>(VkSemaphore(sync.available)),
            reinterpret_cast<uint64_t>(VkSemaphore(sync.draw_complete))
        );

        VKLogicalDevice::native().destroySemaphore(sync.available);
        VKLogicalDevice::native().destroySemaphore(sync.draw_complete);
    }
    for(auto &image : _images) {
        image->destroy_view();
    }
    
    VKLogicalDevice::native().destroy(_swapchain);
}
```

Once all of that compiles, let's actually let our `Engine` create and destroy its very own `Swapchain`. I've added a forward declaration for `Swapchain` and a raw pointer as a private member variable for it within `Engine`.

```cpp
#ifndef VKLEARNIN_ENGINE_ENGINE_HPP
#define VKLEARNIN_ENGINE_ENGINE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Application;
class Swapchain;

class Engine final {
public:
    void render_loop();

    void init();
    void shutdown();

    explicit Engine(Application &app);
    ~Engine();

    Engine() = delete;

    Engine(Engine &&) = delete;
    Engine(const Engine &) = delete;

    Engine & operator=(Engine &&) = delete;
    Engine & operator=(const Engine &) = delete;

private:
    Application &_application;
    Swapchain   *_swapchain;
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_ENGINE_HPP
```

The required steps are then added to `Engine`'s various functions:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/Engine.hpp"

#include "vklearnin/system/Application.hpp"
#include "vklearnin/engine/Swapchain.hpp"

namespace vkl {

// =============================================================================
void Engine::render_loop() {
    using sixty_fps = std::chrono::duration<float, std::ratio<1, 60>>;
    std::this_thread::sleep_for(sixty_fps(60));
}

// =============================================================================
void Engine::init() {
    _swapchain->create();
}

// =============================================================================
void Engine::shutdown() {
    _swapchain->destroy();
}

// =============================================================================
Engine::Engine(Application &app) :
    _application { app },
    _swapchain   { new Swapchain }
{ }

Engine::~Engine() {
    delete _swapchain;
}

} // namespace vkl
```

And we're good to go. Compile and run this bugger. The first of the new output lets us known what the render surface is capable of. In my case, Vulkan supports not only double-buffering, but up to 16 images in a given swapchain.

![[Pasted image 20230114003153.png]]

Next is the simply enormous listing of image format and color space combinations.

![[Pasted image 20230114005135.png]]

And the present modes:

![[Pasted image 20230114005340.png]]

With all of that information gathered, the swapchain is created, the images are requested, image views are created, and the two pairs of semaphores are created.

![[Pasted image 20230114005425.png]]

The output that follows should be the same as the last chapter's, but with the addition of the  two image views and four semaphores we created being destroyed.

![[Pasted image 20230114005505.png]]

