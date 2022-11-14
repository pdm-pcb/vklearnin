### Revisiting the Physical Device
There's one more feature we need to assure the graphics card has for us to be able to use it: the ability to handle Present commands. This implies the presence of a Present Queue. Presentation, in the world of real time graphics, is the act of taking an image which has finished being drawn and handing it over to the monitor for... presentation. =) The only gotcha here is that presentation is dependent upon which kind of surface you're trying to draw to. In order to accomodate that dependency, there's a bit of shuffling to be done.

First, give `GraphicsInstance` a new, public function. I'll be calling it `init_physical_device()`.  It'll take one parameter, which is a `vk::SurfaceKHR`. The implementation of this will be pretty simple for the time being - just take the last three lines from `GraphicsInstance::init()` and plop them into the new function. Add `surface` as a parameter for the call to `PhysicalDevice::select_device()`, then fix up the declaration and definition of that function to match. Don't worry about doing anything with it just yet.

```cpp
void GraphicsInstance::init_physical_device(const vk::SurfaceKHR &surface) {
    _physical_device = new PhysicalDevice();
    _physical_device->init(_graphics_instance);
    _physical_device->select_device(surface);
}
```

Back in `Application`, let's update `init()` to reflect these changes.

```cpp
void Application::init() {
    _graphics_instance = new GraphicsInstance;
    _graphics_instance->init();

    TargetWindow::init();
    TargetWindow::init_surface(_graphics_instance->native());

    _graphics_instance->init_physical_device(TargetWindow::surface());
}
```

When I compile and run this, I get an unused parameter warning, but that's alright for now. I just wante to make sure I didn't miss any of the plumbing. Back to `PhysicalDevice`, then.

Add a new private member variable, `uint32_t _present_queue_index`. Initialize it to `std::numeric_limits<uint32_t>::max()` upon construction so we know what a bogus value will look like. Near the top of `select_device()`, add another `bool` which we'll use to mark support for presenting on this surface. Ultimately, these changes will come down to making a call to `vk::PhysicalDevice::getSurfaceSupportKHR()` for each queue family (and each device) we've got. The first time support for both is found on any single device, that's the one we choose.

Here's what the updated `select_device()` looks like:

```cpp
// In order to render, we need to ensure the graphics card support receiving
// two types of commands: graphics and present. The latter requires an existing
// surface to query, so here we go.
void PhysicalDevice::select_device(const vk::SurfaceKHR &surface) {
    // Reverse sort the devices based on amount of VRAM
    std::sort(_available_devices.begin(), _available_devices.end(),
        [&](const DeviceProperties &a, const DeviceProperties &b) {
            return a.vram_bytes > b.vram_bytes;
        }
    );

    // Set up our hopefully to-be-rectified failure conditions
    bool graphics_support = false;
    bool present_support  = false;
    uint32_t device_index = std::numeric_limits<uint32_t>::max();

    // Loop through all available hardware
    for(uint32_t device = 0; device < _available_devices.size(); ++device) {
        // Ask Vulkan for some details
        const auto &vk_physical_dev = _available_devices[device].device;
        const auto props = vk_physical_dev.getQueueFamilyProperties();

        CONSOLE_TRACE("Found {} queue families for device {}",
                      props.size(), _available_devices[device].name);

        for(uint32_t family = 0; family < props.size(); ++family) {
            // If the current queue family has the graphics bit set, keep track
            if(!graphics_support &&
               props[family].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                graphics_support = true;
                _graphics_queue_index = family;
                device_index = device;
            }

            // If we don't have present support yet, check this queue family
            // against the surface we created earlier
            if(!present_support) {
                auto result =
                    vk_physical_dev.getSurfaceSupportKHR(family, surface);

                // Oblige Vulkan-Hpp and check its return values
                if(result.result != vk::Result::eSuccess) {
                    CONSOLE_CRITICAL("Failed to query surface support.");
                }
                if(result.value) {
                    present_support = true;
                    _present_queue_index = family;
                    device_index = device;
                }
            }

            // No sense going further if we got it
            if(graphics_support && present_support) {
                break;
            }
        }

        // Break now, since the cards are already sorted by VRAM
        if(graphics_support && present_support) {
            CONSOLE_TRACE(
                "Chose queue {} for graphics and {} for presentation",
                _graphics_queue_index,
                _present_queue_index
            );
            break;
        }

        // This device was of no use to us, so reset and continue
        graphics_support = false;
        present_support  = false;
        device_index = std::numeric_limits<uint32_t>::max();
    }

    // This would be most unfortunate
    if(device_index >= _available_devices.size()) {
        CONSOLE_CRITICAL("Could not find a device with support for a graphics "
                         "command queue.");
    }

    // Pluck the winner from the crowd and let's go home
    _physical_device = _available_devices[device_index].device;
    CONSOLE_TRACE("Selected {}", _available_devices[device_index].name);
}
```

If you compile and run, there should be one extra line indicating the actual indices of the queue families we'll be using. In my case, the values are both reliably zero.

One last touch before we leave `PhysicalDevice` for a bit - accessors for our newly stored values. Mine look like this.

```cpp
uint32_t graphics_queue_index() const { return _graphics_queue_index; }
uint32_t present_queue_index()  const { return _present_queue_index;  }
```

...Next!

### Not Quite Queues, Yet
A new class makes its appearence in our project: `CmdQueues`. This single structure will unify the mechanisms we use to send information to the GPU, at least in part. Whether or not the queue families for graphics and present are the same on your hardware, this will account for both.

Starting with the constructor, it'll take one parameter.

```cpp
explicit CmdQueues(const GraphicsInstance &instance);
```

Add in the other five if you wish, as well as the following public member functions:

```cpp
void create_pools();    // Memory from which the queues will be allocated
void create_queues();   // Queues into which commands will be placed
void create_buffers();  // Buffers which shall hold the queued commands
```

A whole host of accessors will also be useful before too long.

```cpp
inline const vk::Queue & graphics_queue() const {
	return _graphics_queue;
}
inline const vk::Queue & present_queue() const {
	return _present_queue;
}
inline const std::vector<vk::DeviceQueueCreateInfo> & create_info() const {
	return _queue_create_info;
}
inline const vk::CommandBuffer & command_buffer() const {
	return _command_buffer;
}
inline const vk::CommandPool & command_pool() const {
	return _command_pool;
}
```

In support of these many functions, we'll need some private member variables.

```cpp
// Creation-related strcutures for the command queues
std::vector<vk::DeviceQueueCreateInfo> _queue_create_info;
std::vector<float> _queue_priorities;

// The only two queue handles we'll need for now
vk::Queue _graphics_queue;
vk::Queue _present_queue;

// Pools from which to allocate the queues, and buffers for the stuffing
// of commands
vk::CommandPool   _command_pool;
vk::CommandBuffer _command_buffer;

// Everybody wants to know about the graphics instance
const GraphicsInstance &_graphics_instance;
```

Flipping over to the implementation, the constructor is a bit more substantial than those we've written so far.

```cpp
CmdQueues::CmdQueues(const GraphicsInstance &instance) :
    _graphics_instance { instance }
{
    // The first thing we'll need is a set of family indices from the physical
    // device. For many graphics cards, these will be the same value. Be that
    // as it may, we ought to use std::set to be sure of how many indices we're
    // working with.
    auto &device = _graphics_instance.physical_device();
    std::set<uint32_t> family_indices {
        device.graphics_queue_index(),
        device.present_queue_index()
    };

    // Set aside the correct amount of space for these two creation data
    // structures
    _queue_create_info.reserve(family_indices.size());
    _queue_priorities.reserve(family_indices.size());

    // And populate said amounts of space
    for(const auto index : family_indices) {
        _queue_priorities.emplace_back(1.0f);
        _queue_create_info.emplace_back(
            vk::DeviceQueueCreateInfo {
                .queueFamilyIndex = index,
                .queueCount = static_cast<uint32_t>(_queue_priorities.size()),
                .pQueuePriorities = _queue_priorities.data(),
            }
        );
    }
}
```

This doesn't actually create any queues, but populates the structures which will inform their creation in a bit. For the moment, we're going to return to devices and establish the logical device.

### Logical Device (Confounding Order)
The final piece of part of the puzzle is now ready to be placed. The logical device requires a little information about the command queues in order to be created. The logical device is ultimately responsible for creating the command queues, but we had to at least ask politely for details beforehand.