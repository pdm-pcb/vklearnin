## The First Few (hundred) Lines
The first concept to discuss and implement with Vulkan is that of the Instance. In OpenGL and D3D11, you have contexts through which you communicate with the GPU. In Vulkan, the Instance is a rough equivalent. From the Instance, we'll be able to query the hardware, inform the windowing platform as to our intention, and so forth. Create a `GraphicsInstance` class.

There will be one public member function (other than the big six) and two private member variables.
```cpp
class GraphicsInstance final {
public:
    void init();

    GraphicsInstance();
    ~GraphicsInstance();

    GraphicsInstance(GraphicsInstance &&other) = delete;
    GraphicsInstance(const GraphicsInstance &other) = delete;

    GraphicsInstance & operator=(GraphicsInstance &&other) = delete;
    GraphicsInstance & operator=(const GraphicsInstance &other) = delete;

private:
    vk::DynamicLoader _loader; // thanks to vulkan-hpp, we can automate a ton
    vk::Instance      _graphics_instance;
};
```

The first is a handle for the dynamic loader provided by the fine folks behind [Vulkan-Hpp](https://github.com/KhronosGroup/Vulkan-Hpp#extensions--per-device-function-pointers). The second is a handle for this class’s namesake. On the implementation side of things, the first step is to add the following macro to the top of the file:
```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/GraphicsInstance.hpp"

// This (and more; see the link) does away with the explicit loading of each
// function/extension
// https://github.com/KhronosGroup/Vulkan-Hpp#extensions--per-device-function-pointers

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
```

This will allow us to bootstrap the loader. In the constructor, ask the loader for the function `vkGetInstanceProcAddr`, then have the dispatcher initialize itself.
```cpp
GraphicsInstance::GraphicsInstance() :
    _graphics_instance { nullptr }

{
    // first step for using the dynamic loader
    using inst_proc = PFN_vkGetInstanceProcAddr;
    inst_proc vkGetInstanceProcAddr =
        _loader.getProcAddress<inst_proc>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

}
```

Moving on to `GraphicsInstance::init()`, we’ll put the application information from `vklearnin.hpp` to use.
```cpp
    // Provide hints about this application to the driver
    vk::ApplicationInfo app_info {
        .pApplicationName = APP_NAME,
        .applicationVersion = APP_VERSION,
        .pEngineName = ENGINE_NAME,
        .engineVersion = ENGINE_VERSION,
        .apiVersion = VK_API_VER
    };
```

Immediately following that, you’ll see the first of what will soon become a  familiar sight. Vulkan likes to know about things in lists. In plain-old-C, there's frequent faffery due to a lack of love for VLAs. With Vulkan-Hpp, we're thankfully permitted to just use vectors. Given that many of the use cases for these is during initialization, there's no worry about heap allocations slowing us down.
```cpp
    std::vector<const char *> enabled_layers;
    std::vector<const char *> enabled_extensions {
        VK_KHR_SURFACE_EXTENSION_NAME
    };
```

Next is a bit of setup that’ll help us down the road. We want to enable the debugging and validation extensions so the driver has a way to tell us if we’ve made some kind of mistake or if we’re leaving performance on the table. I’ll wrap these in `#ifdef`s so we’re not burdened by the extra checks and output while running a release build.
```cpp
#ifdef VKL_DEBUG
    // Here's the first step toward giving the drive a path to keep us abreast
    // of myriad details.
    enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  

    // As with the last line, these features support our debugging efforts
    std::vector<vk::ValidationFeatureEnableEXT> enabled_features {
        vk::ValidationFeatureEnableEXT::eGpuAssisted,
        vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
        vk::ValidationFeatureEnableEXT::eBestPractices,
	    // DebugPrintf and GpuAssisted are mutually exclusive. DebugPrintf is
	    // very handy when used in conjucntion with RenderDoc, but I'm opting
	    // for more self-contained guidance for now.
        // vk::ValidationFeatureEnableEXT::eDebugPrintf,
        vk::ValidationFeatureEnableEXT::eSynchronizationValidation
    };

    // The validation layer helps you know if you've strayed too far from the
    // expected path. It's also extremely opinionated, so each message should
    // be considered individually.
    enabled_layers.push_back("VK_LAYER_KHRONOS_validation");  

    vk::ValidationFeaturesEXT validation_features {
        .enabledValidationFeatureCount =
            static_cast<uint32_t>(enabled_features.size()),
        .pEnabledValidationFeatures = enabled_features.data(),
        .disabledValidationFeatureCount = 0u,
        .pDisabledValidationFeatures = nullptr
    };

#endif // VKL_DEBUG
```

Vulkan surfaces describe the space on screen to which you can draw. They’re also platform dependent, so add the extension you need to our list.
```cpp
#if defined(__linux__)
    enabled_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(_WIN32)
    enabled_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
```

Finally, stitch it all together and call `vk::createInstance()`. Check your result. If the creation was successful, you can call the dynamic dispatcher and let it know we’ve got an instance, so it can populate  whichever function pointers it finds.
```cpp
// Bringing it all together. If we want validation layer functionality, the
// pNext member of vk::InstanceCreateInfo must point to the structure
// assembled above.
vk::InstanceCreateInfo instance_info {

#ifdef VKL_DEBUG
	.pNext = reinterpret_cast<void *>(&validation_features),
#else
	.pNext = nullptr,
#endif // VKL_DEBUG

	.flags = { },
	.pApplicationInfo = &app_info,
	.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size()),
	.ppEnabledLayerNames = enabled_layers.data(),
	.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size()),
	.ppEnabledExtensionNames = enabled_extensions.data()
};

auto result = vk::createInstance(
	&instance_info,
	nullptr,
	&_graphics_instance
);

// If this didn't work, we can go no further.
if(result != vk::Result::eSuccess) {
	CONSOLE_CRITICAL("Failed to create Vulkan instance.");
}

// Inform the dynamic dispatcher that we've got an instance.
VULKAN_HPP_DEFAULT_DISPATCHER.init(_graphics_instance);
```

After the loader has done its work, we need to run through and check that the extensions we requested previously are actually supported. If they are, then we can move on.
```cpp
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
for(const char *required_extension : enabled_extensions) {
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
```

Let’s head back to `Application.hpp` and add the first private member variable. Forward declare the `GraphicsInstance` class, then add a suitably named pointer to that class as a private member. Over in the implementation file, give the pointer an initial value in the constructor. Within `Application::init()`, populate the pointer with a fresh instance of `GraphicsInstance`, then call `GraphicsInstance::init()`. Of course, don’t forget to delete the pointer in `Application::~Application()`.

If you run the program now, you’ll see a couple of extra lines on the terminal. Specifically, the number of instance extensions supported as well as another happy green message informing us that we’ve got a Vulkan 1.1 instance.

## Debug and Validation Output
Before moving on to devices (the graphics card), let’s fire up a helpful supplement to `ConsoleLog`. Create a new class called `VKDebugger`. In the header, add the following function signature as a public, static member.
```cpp
static VKAPI_ATTR vk::Bool32 VKAPI_CALL callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
	VkDebugUtilsMessageTypeFlagsEXT             types,
	const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
	void                                       *user_data
);
```

Add the familiar `init()`/`shutdown()` pair. I've opted to delete the constructor here for clarity.
```cpp
static void init(vk::Instance &instance);
static void shutdown(vk::Instance &instance);

VKDebugger() = delete;
```

This class will also require one private member variable so Vulkan can connect all the dots.
```cpp
static vk::DebugUtilsMessengerEXT _debug_messenger;
```

The implementation illuminates the purpose of this class. When `VKDebugger::init()` is called, we specify which types of feedback we’d like from the API, and provide it the callback we’ll use to parse them.
```cpp
void VKDebugger::init(vk::Instance &instance) {
    vk::DebugUtilsMessengerCreateInfoEXT debug_info {
        .messageSeverity = (
#if defined(__linux__)
			// I find this log level too verbose (ha) on Windows, but it can
            // be helpful on Linux
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
#endif
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo    |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        ),
        .messageType = (
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral    |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        ),
        .pfnUserCallback = VKDebugger::callback
    };

    auto result = instance.createDebugUtilsMessengerEXT(
        &debug_info,
        nullptr,
        &_debug_messenger
    );
    
    if(result != vk::Result::eSuccess) {
        CONSOLE_WARN("Unable to create debug messenger.");
    }
    else {
        CONSOLE_TRACE("Initialized VKDebugger utility");
    }
}
```

The callback itself just decides what color the message should be via the switch statement. I’ve added in some newlines to emphasize errors and warnings, as well as requested the program halt on any error.
```cpp
VKAPI_ATTR vk::Bool32 VKAPI_CALL VKDebugger::callback(
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
            CONSOLE_TRACE("???: {:s}", callback_data->pMessage);
            assert(false);
            break;
    }

    return false;
}
```

Back in `GraphicsInstance.cpp`, add a call to `VKDebugger::init()` once you’ve established a good instance. Correspondingly, in the destructor, add a call to `VKDebugger::shutdown()`. If all goes well, you’ll be able to run the program now and not see any changes. Output should look something like this:
```shell
[11:48:23.065][ 29072 ][vkl::ConsoleLog::init()]: spdlog v1.11.0
[11:48:23.206][ 29072 ][vkl::GraphicsInstance::init()]: Found 13 instance extensions.
[11:48:23.206][ 29072 ][vkl::GraphicsInstance::init()]: Created Vulkan v1.1.0 instance
[11:48:23.207][ 29072 ][vkl::VKDebugger::init()]: Initialized VKDebugger utility

~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
Mission accomplished.
~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
```

If you’d like to see what the output will look like (along with the call to `assert()`) go ahead and comment out the call to `VKDebugger::shutdown()` and run again. You’ll notice it immediately scolds you for not destroying its own member variable by the time the program exits.
```shell
[11:49:40.362][ 1440 ][vkl::ConsoleLog::init()]: spdlog v1.11.0
[11:49:40.518][ 1440 ][vkl::GraphicsInstance::init()]: Found 13 instance extensions.
[11:49:40.518][ 1440 ][vkl::GraphicsInstance::init()]: Created Vulkan v1.1.0 instance
[11:49:40.518][ 1440 ][vkl::VKDebugger::init()]: Initialized VKDebugger utility
[11:49:40.519][ 1440 ][vkl::VKDebugger::callback()]:
Validation Error: [ VUID-vkDestroyInstance-instance-00629 ] Object 0: handle = 0x16abaadb0e0, type = VK_OBJECT_TYPE_INSTANCE; Object 1: handle = 0xfd5b260000000001, type = VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT; | MessageID = 0x8b3d8e18 | OBJ ERROR : For VkInstance 0x16abaadb0e0[], VkDebugUtilsMessengerEXT 0xfd5b260000000001[] has not been destroyed. The Vulkan spec states: All child objects created using instance must have been destroyed prior to destroying instance (https://vulkan.lunarg.com/doc/view/1.3.216.0/windows/1.3-extensions/vkspec.html#VUID-vkDestroyInstance-instance-00629)
```

The debug messages can be quite verbose, but are also super handy when they catch you before you catch yourself.

Next stop, the graphics card!