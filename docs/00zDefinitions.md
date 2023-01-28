Vulkan is complicated. We all know that. In my opinion, part of what makes it difficult to pick up is the terminology. Specifically, the terms which have no direct analog in APIs like OpenGL or Direct3D 11. Let's go over the major pieces of the Vulkan API we'll be dealing with on the way to Hello, Triangle and more.

## Host
In the plainest terms, a host can be considered the CPU and RAM of the machine which executes your program.

## Instance
The type is [vk::Instance](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkInstance.html). As the spec says, this is our first major departure from OpenGL. While the GL context has the (in)convenient trait of being a giant, global, finite state machine, Vulkan opts for an instance, which is distinct per application. It's also thread-aware, so there are situations where you're empowered to delegate chunks of a larger task to different threads.

### Physical Device
A [vk::PhysicalDevice](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPhysicalDevice.html) corresponds with your graphics adapter, whether emulated, integrated, or discrete. From the physical device we can query VRAM amount, driver version, MSAA and mipmap sample support, and more.

## Surface
A [vk::SurfaceKHR](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSurfaceKHR.html) is provided by the platform on which your program runs. The host will provide some portion or the whole of a display where your program may present images. Between the physical device and the surface, things such as color space, image format, and maximum resolution can be queried.

## (Logical) Device
The spec page for [vk::Device](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDevice.html) is a touch sparse, but it's intended to be. The logical device is responsible for managing higher-level concepts than itself, both creation and destruction often times. Multiple logical devices may be created from one physical device.

## Device Queue
As with the logical device, [vk::Queue](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkQueue.html) is simply a handle. The concept of queues is central to the Vulkan paradigm though, so there's some utility here. A given logical device will have a handful of queue families which themselves are able to accept certain types of commands. For example, a queue may accept graphics commands (related to drawing) presentation commands (related to showing an image on screen) or compute commands (related to general purpose GPU programming). Queues may accept more than one type of command, and this ought to be queried on a per-device basis.

## Command Buffer
The medium by which any command is relayed to a given device queue will be an associated [vk::CommandBuffer](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkCommandBuffer.html). Commands are recorded to their respective buffers and then submitted to the relevant queue for eventual execution on the GPU. There are both primary and secondary command buffers, which are used in conjunction when rendering multi-pass scenes.

## Synchronization
As mentioned above, Vulkan is designed to consider applications which will benefit from multiple threads sharing historically monolithic workloads. As stated by [this section](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#synchronization) of the API spec, there are five means by which one can explicitly synchronize the operations of a program. For the purposes of this tutorial, I'll predominately focus on just two: fences and semaphores.

## Fence
A [vk::Fence](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFence.html) is a comparatively crude synchronization primitive which will emit a signal when prompted to do so. A call to [WaitForFences()](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkWaitForFences.html) will halt execution on a given thread until the fence(s) in question emit the required signal.

## Semaphore
Traditionally, a mutex is a **MUT**ually **EX**clusive control on some shared resource, while a Semaphore is a [signal bearer](https://en.wikipedia.org/wiki/Semaphore). Vulkan offers the [vk::Semaphore](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSemaphore.html) as a more flexible signaling mechanism when compared to fences.

## Render Pass
In keeping with modern rendering techniques, Vulkan presumes your program will require some number of passes to render a given scene. A [vk::RenderPass](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkRenderPass.html) is the first layer of this flavorful cake which facilitates such an approach.

## Attachment
A render pass's attachment a collection of traits which fulfill [vk::AttachmentDescription](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkAttachmentDescription.html). The fact that attachments all share one description structure is a little misleading, though - an attachment can be the input or output of a given render pass. A color attachment, for example, is a possible destination for the output of a render pass. By contrast, depth testing requires an attachment which can be written to, but also read from.

## Subpass
Each render pass much consist of at least one subpass, as detailed in one or more [vk::SubpassDescription](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSubpassDescription.html) structures. [Shadow maps](https://en.wikipedia.org/wiki/Shadow_mapping), [bloom](https://en.wikipedia.org/wiki/Bloom_(shader_effect)), and any post-processing effect will require multiple subpasses to complete.

## Render Pass Dependencies
That there may be several subpasses to  a given render pass, it stands to reason that there be an ordering of dependencies between subpasses. This is handled by [vk::SubpassDependency](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSubpassDependency.html), which describes the state of one subpass relative to another as either input or output.

## Pipeline
A [vk::Pipeline](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPipeline.html) handle refers to the steps taken by Vulkan to execute a device queue's worth of command buffers. The [graphics](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkGraphicsPipelineCreateInfo.html) pipeline is of particular interest to begin with - it consists of both programmable and fixed steps that result in an image ready to be presented on screen.

## Resources
Vulkan has two canonical resources: buffers and images.

## Buffer
The [vk::Buffer](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#resources) handle represents some contiguous area of memory wherein one might store vertex, lighting, or transform data. Buffers are organized as one-dimensional arrays and are usually read as literal values or data structures. One may also apply a [vk::BufferView](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkBufferView.html) to support executing image operations on the buffer or some part of it.

## Image
A [vk::Image](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImage.html) may be multidimensional, supporting two or three 'axes'. Unlike buffers, images require additional information to be interpreted. First is the [vk::ImageLayout](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImageLayout.html), which can range from depth stencil to transfer destination to video decoding. These layouts can and frequently do change during operation.

Another companion to images is the [vk::ImageView](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImageView.html). A view is the means by which your program interacts with a given image or subresource of that image (array layers or mipmap levels, for example).

## Descriptors
Descriptors are another layer of representation between the CPU and the GPU regarding shader resources. Descriptors themselves cannot be individually sent to the GPU, however - there must be a [vk::DescriptorSet](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorSet.html) for that. And descriptor sets aren't just declared, but allocated from a [vk::DescriptorPool](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorPool.html).

There are several descriptor types, including image samplers, storage buffers, uniform buffers, and more.

## Swapchain
A [vk::SwapchainKHR](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSwapchainKHR.html) represents an array of images which are able to be presented on a surface. Like surfaces, swapchains are extensions to the Vulkan spec because they are implementation-defined by nature. Aside from presentation, the swapchain is able to notify the application (if requested) when the dimensions or orientation of the display have changed.

## Framebuffer
In Vulkan, the [vk::Framebuffer](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFramebuffer.html) can be thought of as an interface between render passes and images/image views. Together, the framebuffer and render pass(es) will provide the conditions and instructions for the GPU to be able to render the image. The framebuffer is also where blending operations are configured.