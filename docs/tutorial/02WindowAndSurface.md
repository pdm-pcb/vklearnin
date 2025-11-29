## Window
The `TargetWindow` class is a helper class to interface with the [GLFW](https://www.glfw.org/) library for simplifying cross-platform windowing and input handling. In the function `TargetWindow::init()` the following line will cue GLFW that we mean to use Vulkan:

```cpp
::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
```

In `TargetWindow::create()`, we're just creating a window and marking it invisible so it doesn't flicker or flash as we're sorting out our drawing surface.

## Surface
The area of the screen you're going to ask Vulkan to write to the [`vk::SurfaceKHR`](https://docs.vulkan.org/refpages/latest/refpages/source/VkSurfaceKHR.html). To create one, you'll need a create info struct appropriate for your platform (such as [`vk::XlibSurfaceCreateInfoKHR`](https://docs.vulkan.org/refpages/latest/refpages/source/VkXlibSurfaceCreateInfoKHR.html)) and feed it into `vk::Instance::createXlibSurfaceKHR()` or equivalent. The handle you get back will be used later to ensure compatibility between how the surface can display and what you're asking the device to render.

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