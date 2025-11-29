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