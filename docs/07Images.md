## The Image
In Vulkan, an [image](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImage.html) is simply a multidimensional array, with the restriction of having a maximum of three dimensions. This means an image can represent a traditional 2D texture, for example. Images aren't accessed directly, so an [image view](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImageView.html) facilitates this. Images also have associated [formats](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFormat.html) and [layouts](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImageLayout.html) which provide detail regarding the image's intended use. For example, if you want to move data between two images, one should have its layout set to destination and the other set to source, while the formats should generally match up as well.

## Show Yourself
Let's talk about display technology. You're likely familiar with the idea of a display's refresh rate, eg 30Hz, 60Hz, 144Hz, and so on. This metric tells you how many times per second a given display is capable of updating itself, and the display will always redraw itself that many times per second, even if nothing changes. Some mobile hardware will prevent redrawing unchanged data to conserve battery life (and modern displays are capable of varying their refresh rate) but the principle still applies: the display wants to draw images for you at a certain rate.

All the while, the video card is receiving draw commands from an application and doing what's required to build the requested image. Once it's done, it will want to hand the image over to the display. Here's the first way a video card and a display can get out of sync with one another: typically the building of an image inside the GPU will take longer than the  display will take to update itself and present a new image. The display's update process isn't instantaneous, though, so there's hitch number two: the GPU may not have an entire image completed when the display is ready for its next update.

## Staying in Step
There's a strategy that facilitates streaming information like this without interruptions called [multi-buffering](https://en.wikipedia.org/wiki/Multiple_buffering). In real time graphics, double-buffering means we operate on two images simultaneously - one for the GPU to build and one for the display to present to the user. The image below (courtesy of the Wikipedia article linked above) offers a visual representation of the coordination requirements between the GPU and the display.

The monitor signals it's ready for a new image at the same time the graphics card clears out its own memory, preparing to construct the next frame. The darker yellow of "draw A" represents the time taken to execute the draw commands, while the slightly more pale yellow is how long the frame remains on screen before the monitor requests another. Rinse and repeat, with some issues. What if the draw actually takes longer than the monitor's refresh period? Most computer monitors on the market nowadays are at least 60Hz, while a select few can even exceed 500Hz. For a 60Hz display, the graphics card has a little more than 16 milliseconds to complete the frame. If one frame takes 20 milliseconds to draw, this tightly coordinated dance falls apart. In the worst case scenario, this means the area of the screen we're drawing to might actually flicker because there isn't any new information for the display to work with.

To alleviate the worst of the possible missteps with this setup, double-buffering works a treat. The graphics card builds an image while the display presents an image, and around we go. Now if the GPU takes much longer than the monitor to prepare an image, the user is subjected to a more sluggish experience, rather than flickering or blinking on screen.

![[Pasted image 20230113115310.png]]

## Putting it to Use
Let's write some code. We'll need a new class, `Image2D`:

```cpp
#ifndef VKLEARNIN_ENGINE_IMAGE2D_HPP
#define VKLEARNIN_ENGINE_IMAGE2D_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Image2D {
public:
    void init(const vk::Image image, const vk::Format format,
              const vk::ImageLayout layout, const vk::Extent2D &extents);
    void create_view(const vk::ImageAspectFlags &aspect_flags);
    void destroy_view();

    Image2D();
    ~Image2D() = default;

    Image2D(Image2D &&) = delete;
    Image2D(const Image2D &) = delete;

    Image2D & operator=(Image2D &&) = delete;
    Image2D & operator=(const Image2D &) = delete;

private:
    vk::Image       _native;
    vk::ImageView   _view;
    vk::Format      _format;
    vk::ImageLayout _layout;
    vk::Extent2D    _extent;
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_IMAGE2D_HPP
```

The last private member variable is of type `vk::Extent2D`, which is just fancy Vulkan-speak for a single data type representing the width and height of an image, surface, or anything else appropriately described in those terms. There is also a `vk::Extent3D` which carries... you guessed it, a depth value.

For the constructor, I simply have `Image2D` initialize everything in such a way that it's obvious there's no image yet.

```cpp
Image2D::Image2D() :
    _native { nullptr },
    _view   { nullptr },
    _format { vk::Format::eUndefined },
    _layout { vk::ImageLayout::eUndefined },
    _extent { 0u, 0u }
{ }
```

`Image2D::init()` then takes everything Vulkan has provided and our desired attributes and begins fleshing the class out.

```cpp
void Image2D::init(const vk::Image image, const vk::Format format,
                   const vk::ImageLayout layout, const vk::Extent2D &extent)
{
    _native = image;
    _format = format;
    _layout = layout;
    _extent = extent;

    CONSOLE_TRACE(
        "\nStoring image {:#x}:"
        "\n    Extent: {}x{}"
        "\n    Format: {}"
        "\n    Layout: {}",
        reinterpret_cast<uint64_t>(::VkImage(_native)),
        _extent.width, _extent.height,
        to_string(_format),
        to_string(_layout)
    );
}
```

Once the native `vk::Image` and `vk::Format` have been established, it's safe to request an image view from the device.

```cpp
void Image2D::create_view(const vk::ImageAspectFlags &aspect_flags) {
    if(!_native) {
        CONSOLE_CRITICAL("Cannot create view for non-existant image.");
    }
    if(_format == vk::Format::eUndefined) {
        CONSOLE_CRITICAL("Cannot create view for image with undefined format.");
    }

    vk::ImageViewCreateInfo image_info {
        .image = _native,                   // The image handle itself
        .viewType = vk::ImageViewType::e2D, // Image dimension count
        .format = _format,                  // Color format
        .components = {                     
            .r = vk::ComponentSwizzle::eR,  // If color channel values are
            .g = vk::ComponentSwizzle::eG,  // swapped for some reason, these
            .b = vk::ComponentSwizzle::eB,  // paremeters allow us to specify
            .a = vk::ComponentSwizzle::eA,  // which should go where.
        },
        .subresourceRange {
            .aspectMask     = aspect_flags, // Aspect flags describe suitable
                                            // interpretations for this image's
                                            // data
            .baseMipLevel   = 0u,   // Starting mip level
            .levelCount     = 1u,   // Total mip levels
            .baseArrayLayer = 0u,   // Starting array layer
            .layerCount     = 1u    // Total array layers
        }
    };

    auto [result, view] = VKLogicalDevice::native().createImageView(image_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not create image view");
    }
    else {
        CONSOLE_TRACE("Created image view {:#x}",
                      reinterpret_cast<uint64_t>(::VkImageView(view)));
    }
    
    _view = view;
}
```

There are a fair amount of details here that we'll be glossing over until we start working with textures. Suffice it to say that we want our images to have a single level and a single layer, at this point.

Destroying the image view is relatively straightforward:

```cpp
void Image2D::destroy_view() {
    if(!_view) {
        CONSOLE_CRITICAL("Cannot destroy non-existant image view.");
    }
    CONSOLE_TRACE("Destroying image view {:#x}",
                   reinterpret_cast<uint64_t>(::VkImageView(_view)));
    VKLogicalDevice::native().destroy(_view);

    _view = nullptr;
}
```
