A [logical device](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDevice.html) is what most Vulkan code will refer to when it wants to deal with resources controlled by/present on the graphics card. One reason there exists an abstraction between physical and logical devices is that one logical device might actually be made up of more than one physical device. Our code will remain simpler than that, but we still have to deal with the two separate concepts.

The last concept for this chapter is that of device commands. A certain command (for example, a command to draw an object) is stored in a [command buffer](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkCommandBuffer.html). Once the application has finished writing commands to a buffer, that buffer must be submitted to an appropriate [queue](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkQueue.html). Whether or not a queue is appropriate is determined by which queue family it belongs to. Queue families have different [properties](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkQueueFamilyProperties.html), including [flags](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkQueueFlagBits.html), which outline what types of commands their members accept.

Once the application has identified a device queue that accepts the commands you'll be sending, you need to create a command pool within it. The command pool is a dedicated area of device memory from which the application can allocate the aforementioned command buffers. The application will allocate and free (and sometimes reuse) as many different command buffers as it needs to feed the GPU instructions.

## Physical Device
A [physical device](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPhysicalDevice.html) is the Vulkan representation of a single graphics adapter. It might be a discrete GPU (common in gaming desktops and laptops), it might be what's commonly referred to as an integrated GPU (sharing physical space and resources with the CPU), or it might even be a purely emulated device. Any category of physical device gives Vulkan a set of parameters for the next step.

The class wrapping this concept up, `VKPhysicalDevice`, will look like this:

```cpp
#ifndef VKLEARNIN_GRPAHICS_VKPHYSICALDEVICE_HPP
#define VKLEARNIN_GRPAHICS_VKPHYSICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class VKPhysicalDevice final {
public:
    // Keeping a list of these structs will make selecing a physical device
    // easier in the likely even we've got a choice
    struct DeviceProps {
        std::string name  = "";
        size_t vram_bytes = 0;
        std::string driver_version = "";
        std::string vkapi_version  = "";
        vk::PhysicalDevice device = nullptr;
    };

    // Setup functions
    static void query_devices();
    static void select_device();

    // For those who'd like to know
    inline static auto queue_index()    { return _queue_index;     }
    inline static const auto & native() { return _physical_device; }

    // Only one physical device at a time
    VKPhysicalDevice() = delete;

private:
    static std::vector<DeviceProps> _available_devices;
    static vk::PhysicalDevice       _physical_device;

    static uint32_t _queue_index;

    // Adds a device to _available_devices
    static void _store_physical_device(
        const vk::PhysicalDevice &device,
        const vk::PhysicalDeviceProperties &properties,
        const vk::PhysicalDeviceMemoryProperties &memory,
        const vk::PhysicalDeviceDriverProperties &drivers
    );

    // Extra verbose logging for queue families
    static void _print_family_flags(const uint32_t family,
                                    const vk::QueueFlags flags);
};

} // namespace

#endif // VKLEARNIN_GRPAHICS_VKPHYSICALDEVICE_HPP
```

Moving to the definition, I've initialized the static member variables like this:

```cpp
using DeviceList = std::vector<VKPhysicalDevice::DeviceProps>;

DeviceList         VKPhysicalDevice::_available_devices;
vk::PhysicalDevice VKPhysicalDevice::_physical_device { };
uint32_t VKPhysicalDevice::_queue_index = std::numeric_limits<uint32_t>::max();
```

`VKPhysicalDevice::query_devices()` runs through any devices Vulkan knows about and begins gathering details about them.

```cpp
void VKPhysicalDevice::query_devices() {
    // Query and populate the list of physical devices
    auto [enumdev_result, devices] =
        VKInstance::native().enumeratePhysicalDevices();
    if(enumdev_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to enumerate physical devices.");
    }
    CONSOLE_TRACE("Found {} {}", devices.size(),
                  (devices.size() == 1 ? "device" : "devices"));

    for(const auto &device : devices) {
        const auto &props = device.getProperties();

        // We're not interested in software rendering
        if(props.deviceType != vk::PhysicalDeviceType::eDiscreteGpu &&
           props.deviceType != vk::PhysicalDeviceType::eIntegratedGpu)
        {
            continue;
        }

        // We'll want to know what extensions the devices support
        auto [enumext_result, extensions] =
            device.enumerateDeviceExtensionProperties();
        
        if(enumext_result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Failed to enumerate device extensions.");
        }
        CONSOLE_TRACE("Found {} physical device extensions", extensions.size());

        // Here's some extra property gathering so we can establish which of
        // the available devices has the most VRAM
        const auto &memory = device.getMemoryProperties();

        // Yet more property gathering - this time so we can get a driver
        // version string that matches what the hardware vendor publishes
        vk::PhysicalDeviceDriverProperties driver_props { };
        vk::PhysicalDeviceProperties2KHR physical_props2 {
            .pNext = &driver_props
        };
        
        // Run through what extensions we have until we find the driver info
        for(const auto &extension : extensions) {
            if(strcmp(extension.extensionName,
                      VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME) == 0)
            {
                device.getProperties2(&physical_props2);
                break;
            }
        }

        // Hold onto the info we've gathered
        _store_physical_device(device, props, memory, driver_props);
        const auto &properties = _available_devices.back();

        CONSOLE_TRACE(
            "\n"
            "\tDevice Name:    {}\n"
            "\tVRAM:           {} MB\n"
            "\tDriver Version: {}\n"
            "\tVulkan Version: {}\n",
            properties.name,
            properties.vram_bytes / 1000 / 1000,
            properties.driver_version,
            properties.vkapi_version
        );
    }
}
```

`VKPhysicalDevice::select_device()` then goes over the adapters we've found, makes sure to identify devices with command queues we can use, and then chooses the device with the most VRAM by default.

```cpp
// =============================================================================
// In order to render, we need to ensure the graphics card support receiving
// two types of commands: graphics and present. The latter requires an existing
// surface to query, so here we go.
void VKPhysicalDevice::select_device() {
    const auto &surface = TargetWindow::surface();

    // Reverse sort the devices based on amount of VRAM
    std::sort(_available_devices.begin(), _available_devices.end(),
        [&](const DeviceProps &a, const DeviceProps &b) {
            return a.vram_bytes > b.vram_bytes;
        }
    );

    // Set up our hopefully to-be-rectified failure conditions
    std::vector<std::pair<bool, uint32_t>> graphics_support;
    std::vector<std::pair<bool, uint32_t>> present_support;
    graphics_support.resize(_available_devices.size());
    present_support.resize(_available_devices.size());
    std::fill(graphics_support.begin(), graphics_support.end(),
              std::make_pair(false, std::numeric_limits<uint32_t>::max()));
    std::fill(present_support.begin(), present_support.end(),
              std::make_pair(false, std::numeric_limits<uint32_t>::max()));

    // Loop through all available hardware
    for(uint32_t device_idx = 0u;
        device_idx < _available_devices.size();
        ++device_idx)
    {
        // Ask Vulkan for some details
        const auto &gpu = _available_devices[device_idx].device;
        const auto props = gpu.getQueueFamilyProperties();

        CONSOLE_TRACE("Found {} queue families for {}",
                      props.size(), _available_devices[device_idx].name);

        for(uint32_t family = 0u; family < props.size(); ++family) {
            _print_family_flags(family, props[family].queueFlags);

            // If the current queue family has the graphics bit set, keep track
            if(!graphics_support[device_idx].first &&
               props[family].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                graphics_support[device_idx] = { true, family };
            }

            // If we don't have present support yet, check this queue family
            // against the surface we created earlier. getSurfaceSupportKHR()
            // actually just checks whether a given queue family can present
            // on a given surface. Poorly named function, but oh well.
            if(!present_support[device_idx].first) {
                auto result = gpu.getSurfaceSupportKHR(family, surface);

                // Oblige Vulkan-Hpp and check the return value
                if(result.result != vk::Result::eSuccess) {
                    CONSOLE_CRITICAL("Failed to query surface support.");
                }
                if(result.value) {
                    present_support[device_idx] = { true, family };
                }
            }
        }
    }

    uint32_t gfx_queue_index = 0u;
    uint32_t present_queue_index = 0u;

    for(uint32_t device_index = 0u;
        device_index < _available_devices.size();
        ++device_index)
    {
        const bool gfx = graphics_support[device_index].first;
        const uint32_t gfx_fam = graphics_support[device_index].second;

        const bool present = present_support[device_index].first;
        const uint32_t present_fam = present_support[device_index].second;

        vk::PhysicalDeviceFeatures features { };
        _available_devices[device_index].device.getFeatures(&features);

        if(gfx && present && gfx_fam != std::numeric_limits<uint32_t>::max() &&
           present_fam != std::numeric_limits<uint32_t>::max() &&
           features.samplerAnisotropy)
        {
            const auto &dev_store = _available_devices[device_index];
            _physical_device      = dev_store.device;
            gfx_queue_index       = graphics_support[device_index].second;
            present_queue_index   = present_support[device_index].second;

            CONSOLE_TRACE(
                "Selected {}, queue {} for graphics and {} for present",
                dev_store.name,
                gfx_queue_index,
                present_queue_index
            );

            // Just choose the first satisfactory device, as they're already
            // sorted by VRAM
            break;
        }
    }

    // This would be most unfortunate
    if(!_physical_device) {
        CONSOLE_CRITICAL("Could not find a device with support for a graphics "
                         "and present command queues.");
    }
    
    if(gfx_queue_index != present_queue_index) {
        CONSOLE_CRITICAL("Device must support drawing and presenting in a "
                         "single queue.");
    }

    _queue_index = gfx_queue_index;
}
```

As you can see near the end there, it's possible that the graphics queue (which receives draw commands) and the present queue (which handles sending frames to the display) could exist in separate queue families. If that were the case, we'd have to manage two device queues and two command pools. This wouldn't be the end of the world, but I haven't personally encountered this setup in any hardware I've run this code on, so I'll restrict us to working with this probably-more-common setup.

The two remaining helper functions are pretty simple. To start, `_store_physical_device()` has to account for the fact that Vulkan classifies memory into heaps and heaps have their own properties. Since we're looking for VRAM in particular, the heap size we store must have the `vk::MemoryHeapFlagBits::eDeviceLocal` flag set. "Device local" just means the memory is local to the GPU.

```cpp
void VKPhysicalDevice::_store_physical_device(
    const vk::PhysicalDevice &device,
    const vk::PhysicalDeviceProperties &properties,
    const vk::PhysicalDeviceMemoryProperties &memory,
    const vk::PhysicalDeviceDriverProperties &drivers)
{
    DeviceProps store { };
    store.device = device;
    store.name = std::string(properties.deviceName.data());

    size_t vram_bytes = 0;
    for(uint32_t index = 0u; index < memory.memoryHeapCount; ++index) {
        auto flags = memory.memoryHeaps[index].flags;

        if((flags & vk::MemoryHeapFlagBits::eDeviceLocal) == flags) {
            vram_bytes = memory.memoryHeaps[index].size;
            break;
        }
    }
    store.vram_bytes = vram_bytes;

    store.driver_version = std::string(drivers.driverInfo.data());

    store.vkapi_version = fmt::format(
        "{}.{}.{}",
        VK_API_VERSION_MAJOR(properties.apiVersion),
        VK_API_VERSION_MINOR(properties.apiVersion),
        VK_API_VERSION_PATCH(properties.apiVersion)
    );

    _available_devices.push_back(store);
}
```

I decided to wrap the body of `_print_family_flags()` in an preprocessor conditional because, while `CONSOLE_TRACE` could easily be disabled for a release build, the `std::stringstream` shenanigans going on here might slip through and muddy up the works anyway.

```cpp
void VKPhysicalDevice::_print_family_flags(const uint32_t family,
                                           const vk::QueueFlags flags)
{
#ifdef VKL_DEBUG
    std::stringstream flags_stream;
    flags_stream << family << ": ";

    if(flags & vk::QueueFlagBits::eGraphics) {
        flags_stream << "Graphics       ";
    }
    if(flags & vk::QueueFlagBits::eCompute) {
        flags_stream << "Compute        ";
    }
    if(flags & vk::QueueFlagBits::eTransfer) {
        flags_stream << "Transfer       ";
    }
    if(flags & vk::QueueFlagBits::eSparseBinding) {
        flags_stream << "Sparse Binding ";
    }
    if(flags & vk::QueueFlagBits::eProtected) {
        flags_stream << "Protected      ";
    }

    CONSOLE_TRACE("    {}", flags_stream.str());
#endif // VKL_DEBUG
}
```

If all of that compiles just fine, let's test it.

## Initial Testing
Add a public function to `VKInstance` - `create_device()`.

```cpp
void VKInstance::create_device() {
    VKPhysicalDevice::query_devices();
    VKPhysicalDevice::select_device();
}
```

Don't forget to include `VKPhysicalDevice.hpp`, then add one line to `Application::_init()`:

```cpp
void Application::_init() {
    ConsoleLog::init();
    VKInstance::init();
    TargetWindow::spawn_window();
    TargetWindow::create_surface();
    VKInstance::create_device();
}
```

With those changes in place, you should see something like this when you run your code.

![[Pasted image 20230108061818.png]]

Take a close look at all of this new output and check that it makes sense. Easy ones to check would be VRAM and driver version. If you've got a machine with GPUs made by two different vendors, it's also interesting to see how the queue family layouts differ.

If everything's in order, it's time to massage our physical device choice into a logical device.

## Device Queue and Command Pool
I'll keep this section brief, as the classes we're writing don't bear a great deal of responsibility.

Starting with the queue:

```cpp
#ifndef VKLEARNIN_GRPAHICS_VKQUEUE_HPP
#define VKLEARNIN_GRPAHICS_VKQUEUE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class VKQueue final {
public:
    void fill_create_info(const uint32_t index, const float priority = 1.0f);
    void request_queue();

    inline const auto & native()      const { return _queue;       }
    inline const auto index()         const { return _index;       }
    inline const auto priority()      const { return _priority;    }
    inline const auto & create_info() const { return _create_info; }

    VKQueue();
    ~VKQueue() = default;

    VKQueue(VKQueue &&) = delete;
    VKQueue(const VKQueue &) = delete;

    VKQueue & operator=(VKQueue &&) = delete;
    VKQueue & operator=(const VKQueue &) = delete;

private:
    uint32_t _index;
    float    _priority;
    vk::DeviceQueueCreateInfo _create_info;

    vk::Queue _queue;
};

} // namespace vkl


#endif // VKLEARNIN_GRPAHICS_VKQUEUE_HPP
```

Of note here is that different device queues can have assigned priorities, from 0.0 to 1.0. This will affect the order of their execution on the device. Needless to say, this level of optimization is not currently useful.

`VKQueue`'s source file is shorter than its header file:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/graphics/VKQueue.hpp"

#include "vklearnin/graphics/VKLogicalDevice.hpp"

namespace vkl {

// Provide information to the DeviceQueueCreateInfo struct so VKLogicalDevice
// can use it during its own creation process
void VKQueue::fill_create_info(const uint32_t index, const float priority) {
    _index    = index;
    _priority = priority;

    _create_info = vk::DeviceQueueCreateInfo {
        .queueFamilyIndex = _index,
        .queueCount       = 1u,
        .pQueuePriorities = &_priority,
    };
}

// Pretty please
void VKQueue::request_queue() {
    _queue = VKLogicalDevice::native().getQueue(_index, 0u);
    if(!_queue) {
        CONSOLE_CRITICAL("Could not get device queue");
    }
}

VKQueue::VKQueue() :
    _queue { }
{ }

} // namespace vkl
```

The fact that `fill_create_info()` and `request_queue()` are separate might seem weird, but it's all in service of the order in which Vulkan wants things done. In the spirit of holding our noses until it makes sense, here's `VKCmdPool`:

```cpp
#ifndef VKLEARNIN_GRPAHICS_VKCMDPOOL_HPP
#define VKLEARNIN_GRPAHICS_VKCMDPOOL_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class VKCmdPool final {
public:
    void reset(const vk::CommandPoolResetFlags flags = { }) const;

    void create();
    void destroy();

    inline const auto & native() const { return _pool; }

    VKCmdPool() = default;
    ~VKCmdPool() = default;

    VKCmdPool(VKCmdPool &&) = delete;
    VKCmdPool(const VKCmdPool &) = delete;

    VKCmdPool & operator=(VKCmdPool &&) = delete;
    VKCmdPool & operator=(const VKCmdPool &) = delete;

private:
    vk::CommandPool _pool;
};

} // namespace vkl


#endif // VKLEARNIN_GRPAHICS_VKCMDPOOL_HPP
```

And accompanying source:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/graphics/VKCmdPool.hpp"

#include "vklearnin/graphics/VKLogicalDevice.hpp"
#include "vklearnin/graphics/VKQueue.hpp"

namespace vkl {

// Clear any contents of this command pool. Command buffers allocated from this
// pool will need to be re-allocated.
void VKCmdPool::reset(const vk::CommandPoolResetFlags flags) const {
    VKLogicalDevice::native().resetCommandPool(_pool, flags);
}

void VKCmdPool::create() {
    vk::CommandPoolCreateInfo pool_info {
        .queueFamilyIndex = VKLogicalDevice::cmd_queue().index()
    };

    auto result = VKLogicalDevice::native().createCommandPool(
        &pool_info,
        nullptr,
        &_pool
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create command pool.");
    }
    else {
        CONSOLE_TRACE(
            "Created command pool {:#x}.",
            reinterpret_cast<uint64_t>(VkCommandPool(_pool))
        );
    }
}

void VKCmdPool::destroy() {
    CONSOLE_TRACE(
        "Destroying command pool {:#x}.",
        reinterpret_cast<uint64_t>(VkCommandPool(_pool))
    );
    VKLogicalDevice::native().destroyCommandPool(_pool);
}

} // namespace vkl
```

Provided everything compiles, those classes should be good to go and we're clear to actually implement them in our own logical device.

## Logical Device
The declaration for `VKLogicalDevice` is refreshingly brief.

```cpp
#ifndef VKLEARNIN_GRPAHICS_VKLOGICALDEVICE_HPP
#define VKLEARNIN_GRPAHICS_VKLOGICALDEVICE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class VKQueue;
class VKCmdPool;

class VKLogicalDevice final {
public:
    static void create();
    static void destroy();

    inline static const auto & native()    { return _logical_device; }
    inline static const auto & cmd_queue() { return _cmd_queue; }
    inline static const auto & cmd_pool()  { return _cmd_pool; }

    VKLogicalDevice() = delete;

private:
    static VKQueue    _cmd_queue;
    static VKCmdPool  _cmd_pool;
    static vk::Device _logical_device;
};

} // namespace

#endif // VKLEARNIN_GRPAHICS_VKLOGICALDEVICE_HPP
```

`VKLogicalDevice::create()` will look like this:

```cpp
void VKLogicalDevice::create() {
    // The first step in device creation is to tell the queue what index it
    // will be using.
    _cmd_queue.fill_create_info(VKPhysicalDevice::queue_index());

    // Next, ask the queue for the populated structure
    vk::DeviceQueueCreateInfo queue_info[] {
        _cmd_queue.create_info()
    };

    // Provided this is a debug build, we'll want to include the validation
    // layer to afford ourselves the largest possible safety net during
    // development
    std::vector<const char *> layers;
#ifdef VKL_DEBUG
        layers.push_back("VK_LAYER_KHRONOS_validation");
#endif // VKL_DEBUG

    // As this is a graphics application, enable the swapchain extension for
    // whatever surface we're using
    std::vector<const char *> extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // That should be everything we need
    vk::DeviceCreateInfo device_info {
        .queueCreateInfoCount    = static_cast<uint32_t>(std::size(queue_info)),
        .pQueueCreateInfos       = queue_info,
        .enabledLayerCount       = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames     = layers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    // Attempt creation
    auto result = VKPhysicalDevice::native().createDevice(
        &device_info,
        nullptr,
        &_logical_device
    );

    // Make sure it worked
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create logical device.");
    }
    else {    
        CONSOLE_TRACE(
            "Created logical device {:#x}",
            reinterpret_cast<uint64_t>(VkDevice(native()))
        );
    }

    // Once the logical device is established, the queue and pool can likewise
    // come online
    _cmd_queue.request_queue();
    _cmd_pool.create();

    // This is the final step in providing the dynamic loader with information
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_logical_device);
}
```

The last line of `create()` is also the last step in leveraging VulkanHpp's dynamic loader. Now we've got access to everything the API has to offer, and we never had to touch a function pointer to get it. =)

Finally, `VKLogicalDevice::destroy()` is nice and short:

```cpp
void VKLogicalDevice::destroy() {
    _cmd_pool.destroy();

    CONSOLE_TRACE(
        "Destroying logical device {:#x}",
        reinterpret_cast<uint64_t>(VkDevice(native()))
    );

    _logical_device.destroy();
}
```

## Integration and Testing
`VKInstance` will now need `VKLogicalDevice.hpp` included, as well as one new function and two more lines. Specifically:

```cpp
// =============================================================================
void VKInstance::create_device() {
    VKPhysicalDevice::query_devices();
    VKPhysicalDevice::select_device();
    VKLogicalDevice::create();
}

// =============================================================================
void VKInstance::destroy_device() {
    VKLogicalDevice::destroy();
}
```

`Application::_shutdown()` also gains a line.

```cpp
void Application::_shutdown() {
    VKInstance::destroy_device();
    TargetWindow::destroy_surface();
    VKInstance::shutdown();
}
```

All of that should compile and run, giving you something like this for output:

![[Pasted image 20230108065933.png]]

The only new material here should be the device- and queue-related objects accounting for themselves. If what you're seeing seems right to you, let's move on!