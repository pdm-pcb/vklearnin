## The Vulkan Instance
Vulkan has no global state, so each application needs to track its own. The instance will store this information. First, let's look at the member variables for the class.

```cpp
vk::Instance _handle  { nullptr };

std::string _app_name { };
std::uint32_t _app_version { 0u };

vk::DynamicLoader         _loader { };
vk::ApplicationInfo       _app_info { };
std::vector<char const *> _enabled_layers { };
std::vector<char const *> _enabled_extensions { };

std::vector<vk::ValidationFeatureEnableEXT>  _vvl_enabled { };
std::vector<vk::ValidationFeatureDisableEXT> _vvl_disabled { };
vk::ValidationFeaturesEXT                    _vvl_features { };

vkDebugMessenger _debug_messenger { };
```

[`_handle`](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkInstance.html) represents the instance itself. `_app_name` and `_app_version` are details we'll use to populate other fields in the near future.

`_loader` is a part of [Vulkan-Hpp](https://github.com/KhronosGroup/Vulkan-Hpp) that will handle locating function pointers for us automatically.

The remaining members are intended to populate a [`vk::InstanceCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkInstanceCreateInfo.html) structure.

[`_app_info`](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkApplicationInfo.html) is a structure that holds details about our app that might be used by the driver as hints for possible optimization. One situation in which this could be useful is when using a well known or widely used commercial game engine. `_app_info` also stores the version of the Vulkan API we're requesting.

`_enabled_layers` and `_enabled_extensions` will be lists of [layers](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#extendingvulkan-layers) and [extensions](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#extendingvulkan-extensions) we are requesting from the driver. The only layer we'll use will be for validation in debug builds. As for instance extensions, there well always be at least two: [`VK_KHR_surface`](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_surface.html) and a platform-specific surface extension, such as [`VK_KHR_xlib_surface`](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_xlib_surface.html). Together, these two extensions allow us to create a [`vk::Surface`](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSurfaceKHR.html) in an environment using the X Window system. Windows and other platforms have similar combinations required for drawing.

The member variables starting with `_vvl` deal with the Vulkan Validation Layers, which allow the driver to check our work during run time.

`_debug_messenger` is a helper class for the [debug utils messenger](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDebugUtilsMessengerEXT.html), which is how Vulkan can send us messages about what's going on during run time.

## Debug Messenger
This is the means by which you give the validation layers and drivers generally a voice. There's one function with a specific signature that's provided to the API:

```cpp
static VKAPI_ATTR vk::Bool32 VKAPI_CALL messenger(
    vk::DebugUtilsMessageSeverityFlagBitsEXT      severity,
    vk::DebugUtilsMessageTypeFlagsEXT             types,
    vk::DebugUtilsMessengerCallbackDataEXT const *callback_data,
    void                                         *user_data
);
```

And in the body of that function, you can write to the console, a log file, or whatever you see fit.

## Window and Surface
The `TargetWindow` class is a helper class to interface with the [GLFW](https://www.glfw.org/) library for simplifying cross-platform windowing and input handling. In the function `TargetWindow::init()` the following line will cue GLFW that we mean to use Vulkan:

```cpp
::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
```

In `TargetWindow::create()`, we're just creating a window and marking it invisible so it doesn't flicker or flash as we're sorting out our drawing surface.

`vkSurface::create()` populates a surface create info struct appropriate for your platform (such as [`vk::XlibSurfaceCreateInfoKHR`](https://docs.vulkan.org/refpages/latest/refpages/source/VkXlibSurfaceCreateInfoKHR.html)) and calls `vk::Instance::createXlibSurfaceKHR()` or equivalent.

## Physical Device
Generally, the dedicated silicon for graphics calculations and the associated drivers will be represented via a [`vk::PhysicalDevice`](https://docs.vulkan.org/refpages/latest/refpages/source/VkPhysicalDevice.html). Enumerating the available devices will allow us to check what functionality is supported, how much VRAM we're working with, and the like. Our first step is to populate a list of devices such that we might choose from them. There are three categories determining suitability: queue families, device features, and device extensions.
##### Queue Families
The mechanism by which you can get Vulkan to do useful things for you is populating and submitting a [`vk::Queue`](https://docs.vulkan.org/refpages/latest/refpages/source/VkQueue.html). Queues come in families, which are suitable for different tasks. Specifically, we want to check the [`vk::QueueFamilyProperties::queueFlags`](https://docs.vulkan.org/refpages/latest/refpages/source/VkQueueFlagBits.html) field for the graphics flag. In addition, we must ask the physical device if it's able to support the surface we created earlier. The first queue family that meets these two requirements will be the queue index we select.
##### Features and Extensions
Different physical devices (and their Vulkan implementations) will support different features of the API, as well as special extensions to it. We can query a physical device to see if it supports our requirements.

## Surface Details
With a physical device selected, we can check the finer details of how the device and our existing surface will interact.
##### Surface Capabilities
Defining characteristics like  image counts, resolution, and more can be found in the [`vk::SurfaceCapabilitiesKHR`](https://docs.vulkan.org/refpages/latest/refpages/source/VkSurfaceCapabilitiesKHR.html) struct. We want to make sure these are sane, and note the values for future use.
##### Surface Formats
Two more properties we care about are the surface's image format and display color space. Together, those two populate a [`vk::SurfaceFormatKHR`](https://docs.vulkan.org/refpages/latest/refpages/source/VkSurfaceFormatKHR.html). Surfaces commonly support several formats, so it is worth checking that the one you're interested in using is supported.
##### Present Modes
The way the device and display adapter will handle image presentation is the final measure of compatibility we'll require of the surface we're working with. The list of options is defined in [`vk::PresentModeKHR`](https://docs.vulkan.org/refpages/latest/refpages/source/VkPresentModeKHR.html), and the surface will tell us which of those it can handle.