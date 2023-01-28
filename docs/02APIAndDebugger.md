## The First Few (hundred) Lines
The first concept to discuss and implement with Vulkan is that of the Instance. In OpenGL and D3D11, you have contexts through which you communicate with the GPU. In Vulkan, the Instance does much of what a context might do, but more broadly serves as an interface to the GPU. From our Instance, we'll be able to query the hardware, inform the windowing platform of our intentions, and so forth. Let's create a `VKInstance` class.

```cpp
#ifndef VKLEARNIN_SYSTEM_VKINSTANCE_HPP
#define VKLEARNIN_SYSTEM_VKINSTANCE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class VKInstance final {
public:
    using ValidationFeatures = std::vector<vk::ValidationFeatureEnableEXT>;

    static void init();
    static void shutdown();

    inline static const vk::Instance & native() { return _instance; }

    VKInstance() = delete;
    ~VKInstance() = delete;

    VKInstance(VKInstance &&other) = delete;
    VKInstance(const VKInstance &other) = delete;

    VKInstance & operator=(VKInstance &&other) = delete;
    VKInstance & operator=(const VKInstance &other) = delete;

private:
    // Thanks to Vulkan-hpp, we can automate the loading of zillions of
    // function pointers
    static vk::DynamicLoader _loader;
    static vk::Instance      _instance;

    static vk::ApplicationInfo       _app_info;
    static std::vector<const char *> _enabled_layers;
    static std::vector<const char *> _enabled_extensions;
    static ValidationFeatures        _validation_features;
    static vk::ValidationFeaturesEXT _validation_extensions;

    static vk::InstanceCreateInfo _instance_create_info;

    static void _init_dynamic_loader();
    static void _init_app_info();
    static void _init_layers();
    static void _init_extensions();
};

} // namespace vkl
#endif // VKLEARNIN_SYSTEM_VKINSTANCE_HPP
```

The first member variable is a handle for the dynamic loader provided by the fine folks behind [Vulkan-Hpp](https://github.com/KhronosGroup/Vulkan-Hpp#extensions--per-device-function-pointers). The second is a handle for this class’s namesake. On the implementation side of things, the first step is to add the following macro to the top of the file, followed by the initializations for each static member.

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/VKInstance.hpp"

// This (and more; see the link) does away with the explicit loading of each
// function/extension
// https://github.com/KhronosGroup/Vulkan-Hpp
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace vkl {

vk::DynamicLoader              VKInstance::_loader   { };
vk::Instance                   VKInstance::_instance { };
vk::ApplicationInfo            VKInstance::_app_info { };
std::vector<const char *>      VKInstance::_enabled_layers;
std::vector<const char *>      VKInstance::_enabled_extensions;
VKInstance::ValidationFeatures VKInstance::_validation_features;
vk::ValidationFeaturesEXT      VKInstance::_validation_extensions { };
vk::InstanceCreateInfo         VKInstance::_instance_create_info  { };
```

The first member function to receive our attention in this file is `init()`, which itself will call private member functions alluded to in the header.

```cpp
void VKInstance::init() {    
    _init_dynamic_loader(); // The first step for using the dynamic loader
    _init_app_info();       // Provide hints about this program to the driver
    _init_layers();         // There are many layers. Validation is our favorite
    _init_extensions();     // Extensions are often implementation defined

    // Bringing it all together. If we want validation layer functionality, the
    // pNext member of vk::InstanceCreateInfo must point to the structure
    // assembled above.
    vk::InstanceCreateInfo instance_info {
#ifdef VKL_DEBUG
        .pNext = reinterpret_cast<void *>(&_validation_extensions),
#else
        .pNext = nullptr,
#endif // VKL_DEBUG
        .flags = { },
        .pApplicationInfo = &_app_info,
        .enabledLayerCount = 
            static_cast<uint32_t>(_enabled_layers.size()),
        .ppEnabledLayerNames = _enabled_layers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(_enabled_extensions.size()),
        .ppEnabledExtensionNames = _enabled_extensions.data()
    };

    auto result = vk::createInstance(
        &instance_info,
        nullptr,
        &_instance
    );

    // If this didn't work, we can go no further.
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to create Vulkan instance."
                         "\n\t'{}'", to_string(result));
    }

    // Inform the dynamic dispatcher that we've got an instance.
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_instance);

    // Disabling constructors in Vulkan-Hpp forces the programmer to check the
    // return status of any function which offers one. So we'll check that
    // before proceeding to utilize the other return value.
    auto [ext_result, extensions] = vk::enumerateInstanceExtensionProperties();
    if(ext_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to enumerate instance extensions.");
    }

    CONSOLE_TRACE("Found {} instance extensions.", extensions.size());   

    // At most, three instance extensions are required at this point. Run
    // through all extensions the driver offers and make sure we've got what
    // we need
    for(const char *required_extension : _enabled_extensions) {
        bool supported = false;
        for(const auto &extension : extensions) {
            if(strcmp(required_extension, extension.extensionName) == 0) {
                supported = true;
                break;
            }
        }
        if(!supported) {
            CONSOLE_CRITICAL(
                "Instance extension '{}' unsupported",
                required_extension
            );
        }
    }

    CONSOLE_INFO(
        "Created Vulkan v{}.{}.{} instance",
        VK_API_VERSION_MAJOR(VK_API_VER),
        VK_API_VERSION_MINOR(VK_API_VER),
        VK_API_VERSION_PATCH(VK_API_VER)
    );
}
```

`VKInstance::shutdown()` is short and sweet:

```cpp
void VKInstance::shutdown() {
    _instance.destroy();
}
```

The first private function called by `init()` bootstraps `Vulkan-Hpp`'s auto-loader.

```cpp
void VKInstance::_init_dynamic_loader() {
    using inst_proc = PFN_vkGetInstanceProcAddr; // A little brevity
    auto vkGetInstanceProcAddr = _loader.getProcAddress<inst_proc>(
        "vkGetInstanceProcAddr"
    );
    // Bootstrap the auto-loader
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}
```

Next we make use of some of the information provided in `vklearnin.hpp`.

```cpp
// Several static constexpr values loaded in from the central header
void VKInstance::_init_app_info() {
    _app_info.pApplicationName   = APP_NAME;
    _app_info.applicationVersion = APP_VERSION;
    _app_info.pEngineName        = ENGINE_NAME;
    _app_info.engineVersion      = ENGINE_VERSION;
    _app_info.apiVersion         = VK_API_VER;
}
```

The last of the prep work for the Instance is layers and extensions.

```cpp
void VKInstance::_init_layers() {
#ifdef VKL_DEBUG
    // The validation layer helps you know if you've strayed too far from the
    // expected path. It's also extremely opinionated, so each message should
    // be considered individually.
    _enabled_layers = { "VK_LAYER_KHRONOS_validation" };
#endif // VKL_DEBUG
}

void VKInstance::_init_extensions() {
    _enabled_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    // Surfaces describe the spaces to which you can draw in Vulkan. They're
    // also platform dependant.
#if defined(__linux__)
    _enabled_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
    _enabled_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef VKL_DEBUG
    // The first steps toward giving the drive a path to keep us abreast of
    // myriad details.
    _enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    _enabled_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    // As with the last line, these features support our debugging efforts
    _validation_features = {
        vk::ValidationFeatureEnableEXT::eGpuAssisted,
        vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
        vk::ValidationFeatureEnableEXT::eBestPractices,
        // DebugPrintf and GpuAssisted are mutually exclusive. DebugPrintf is
        // very handy when used in conjucntion with RenderDoc, but I'm opting
        // for more self-contained guidance for now.
        // vk::ValidationFeatureEnableEXT::eDebugPrintf,
        vk::ValidationFeatureEnableEXT::eSynchronizationValidation
    };
#endif // VKL_DEBUG

    _validation_extensions = {
        .enabledValidationFeatureCount =
            static_cast<uint32_t>(_validation_features.size()),
        .pEnabledValidationFeatures = _validation_features.data(),
        .disabledValidationFeatureCount = 0u,
        .pDisabledValidationFeatures = nullptr
    };

    for(const auto &extension : _enabled_extensions) {
        CONSOLE_TRACE("Requesting instance extension '{}'", extension);
    }
}
```

To but all of this to use and prove out our first Vulkan-specific code, be sure to add an `#include` directive to `vklearnin.hpp` or your equivalent.

```cpp
// #define MEMLOG
#include "vklearnin/tools/MemTracker.hpp"
#include "vklearnin/tools/ConsoleLog.hpp"
#include "vklearnin/tools/Timekeeper.hpp"

#include "vklearnin/system/Application.hpp"
#include "vklearnin/system/VKInstance.hpp"
```

Then, simply modify `Applicaiton`'s constructor to initialize the Instance.

```cpp
Application::Application() :
    _engine { new Engine(*this) }
{
    ConsoleLog::init();
    VKInstance::init();
}
```

When I compile and run the code on Windows, I get this as output. It may not seem like much, but verifying these instance extensions is foundational for everything to come.

![[Pasted image 20221228235252.png]]

The extra `MemTracker` banner pops up because of the `vk::DynamicLoader` member within `VKInstance`. You can throw in some break points if you wish to explore further. With that oddity accounted for however, I vote we're good to move on to the next class designed to simplify Vulkan development.

## VKDebugger
Vulkan offers a [Debug Utilities extension](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_EXT_debug_utils.html) which makes it very easy to keep an eye on the API and listen when it wants to tell you something. All we need to do is give it a function signature of the expected type and react accordingly when it's called. Here's the definition for how I go about leveraging this handy extension:

```cpp
#ifndef VKLEARNIN_SYSTEM_VKDEBUGGER_HPP
#define VKLEARNIN_SYSTEM_VKDEBUGGER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class VKDebugger final {
public:
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL messenger(
        VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
        VkDebugUtilsMessageTypeFlagsEXT             types,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void                                       *user_data
    );

    static void init(vk::Instance &instance);
    static void shutdown(vk::Instance &instance);

    VKDebugger() = delete;

private:
    static vk::DebugUtilsMessengerEXT _debug_messenger;
};

} // namespace vkl
#endif // VKLEARNIN_SYSTEM_VKDEBUGGER_HPP
```

Within `VkInstance::init()`, add a call to the debugger's initialization function, and don't forget to add the debugger's header. I chose to put the new code at the very end of the function.

```cpp
//...
    CONSOLE_INFO(
        "Created Vulkan v{}.{}.{} instance",
        VK_API_VERSION_MAJOR(VK_API_VER),
        VK_API_VERSION_MINOR(VK_API_VER),
        VK_API_VERSION_PATCH(VK_API_VER)
    );

#ifdef VKL_DEBUG
    VKDebugger::init(_instance);
#endif // VKL_DEBUG
}
```

Correspondingly, `VkInstance::shutdown()` gets a little more interesting:

```cpp
// =============================================================================
void VKInstance::shutdown() {
#ifdef VKL_DEBUG
    VKDebugger::shutdown(_instance);
#endif // VKL_DEBUG

    _instance.destroy();
}
```

I'm electing to have this `init()`/`shutdown()` pair called only during debug builds in hopes of keeping a slimmer console log when I'm actually worried about performance.

Turning to the implementation file now, `init()` will set the severity filter, message type filter, and function pointer for our logging callback.

```cpp
void VKDebugger::init(vk::Instance &instance) {
    // Populate the create info struct with the severity levels we're
    // interested in, the types we're interested in, and offer a callback
    // pointer to the API
    vk::DebugUtilsMessengerCreateInfoEXT messenger_info {
        .messageSeverity = (
#if defined(VKL_LINUX)
			// I find this log level too verbose (ha) on Windows, but it can
            // be helpful on Linux
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
#endif
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo    |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        ),
        .messageType = (
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral     |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation  |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
        ),
        .pfnUserCallback = VKDebugger::messenger
    };

    // Give it a shot
    auto result = instance.createDebugUtilsMessengerEXT(
        &messenger_info,
        nullptr,
        &_debug_messenger
    );
    
    // React accordingly
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create debug messenger."
                         "\n\t'{}'", to_string(result));
    }
    else {
        CONSOLE_TRACE(
            "Created VKDebugger messenger {:#x}",
            reinterpret_cast<uint64_t>(
                VkDebugUtilsMessengerEXT(_debug_messenger)
            )
        );
    }
}
```

`VKDebugger::destroy()` is suitably brief:

```cpp
void VKDebugger::shutdown(vk::Instance &instance) {
    CONSOLE_TRACE(
        "Destroying VKDebugger messenger {:#x}",
            reinterpret_cast<uint64_t>(
                VkDebugUtilsMessengerEXT(_debug_messenger)
            )
    );
    instance.destroy(_debug_messenger);
}
```

In both of the above function bodies, you may have noticed I'm casting the `vk::DebugUtilsMessengerEXT` handle to a `uint64_t` after first casting it to the underlying Vulkan type `VkDebugUtilsMessengerEXT`. Any non-dispatchable Vulkan handle is a unsigned, 64-bit integer under the hood, which allows much easier tracking of individual entities. Not all things you want to identify can be so easily tagged, so I use this method where I can. I'm also formatting the value in hex because I find that easier to read when it comes to identifiers.

Finally, the useful bit of our debugger is in the callback we provided to Vulkan in the first place.

```cpp
VKAPI_ATTR vk::Bool32 VKAPI_CALL VKDebugger::messenger(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT types,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        [[maybe_unused]] void *user_data)
{
    switch(severity) {
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            CONSOLE_TRACE("{:s}", callback_data->pMessage);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            CONSOLE_INFO("{:s}", callback_data->pMessage);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            CONSOLE_WARN("\n{:s}\n", callback_data->pMessage);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            CONSOLE_ERROR("\n{:s}\n", callback_data->pMessage);
            assert(false);
            break;
        default:
            CONSOLE_CRITICAL("???: {:s}", callback_data->pMessage);
            assert(false);
            break;
    }

    return false;
}
```

The `user_data` parameter is empty of our own design. I've also elected to ignore the message type flag bits, because that information frequently winds up in `callback_data->pMessage` anyway, so it'll just get duplicated on screen one way or another.

With those changes in place, compiling and running should yield something akin to the following:

![[Pasted image 20230108033928.png]]

If you want to see the debugger in action, simply destroy the Vulkan Instance before calling `VKDebugger::shutdown()` and nod knowingly as the API scolds you.

```cpp
void VKInstance::shutdown() {
    _instance.destroy();

#ifdef VKL_DEBUG
    VKDebugger::shutdown(_instance);
#endif // VKL_DEBUG
}
```

![[Pasted image 20230108034021.png]]

If the tool works as expected, it's time to wade still deeper in the muck and integrate your platform's Window Manager into the code base.