Thus far, we've built up what's required to define an area of a display to which we are  able to draw. When there's an image ready to be shown, the swapchain (and present queue) will make sure it gets there. Now we need the supporting the GPU to build that image. The [render pass](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkRenderPass.html) is a fundamental building blocks of that process.

## One Pass Two Pass, Red Pass Blue Pass
A render pass must contain at least one subpass. The names are a little convoluted in this regard, wherein a render pass may be considered complete with only one subpass. But, in the event you want to do something snazzy like write graphics code that'll run well on [mobile GPUs](https://developer.samsung.com/galaxy-gamedev/resources/articles/gpu-framebuffer.html), then the vocabulary starts to seem a little less silly.

In the event you're writing a render pass which does require multiple subpasses, then you'd concern yourself with the next aspect of a render pass: [subpass dependencies](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSubpassDependency.html). Briefly, these are the means by which you can tell Vulkan one subpass depends upon another, ensuring the work will happen in the required order. Since we're sticking with single subpass render passes, I'll just leave it at that.

## Attachments
The final requirement of a render pass is the attachments. To begin with, think of an attachment as an image view like we've seen before. Like an image view offers additional information on how to use the block of memory to which it refers, an attachment describes an image in terms that a render pass can make use of.

## Framebuffers
While render passes themselves need to know what to expect in terms of their attachments, the object that'll actually contain the rest of a given image's meta-data is the [framebuffer](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFramebuffer.html).

## Back to the Code
As we covered previously, a physical device usually has more than one queue family which are each capable of processing different types of commands. In our case, the graphics and present command queues are the two we're most interested in. Then the logical device created from that physical device will offer us command pools, from which command buffers may be allocated. These command buffers are what the host application will fill with work and then submit to the GPU.

Now that we're ready to construct a render pass, we'll be needing these pools and buffers. Let's start by creating the `CmdBuffer` object. It's declaration will look like this:

```cpp
#ifndef VKLEARNIN_RENDER_RENDERPASS_CMDBUFFER_HPP
#define VKLEARNIN_RENDER_RENDERPASS_CMDBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class CmdBuffer final {
public:
    void allocate(const vk::CommandBufferAllocateInfo &buffer_info);

    inline const auto & native() const { return _buffer; }

    CmdBuffer();
    ~CmdBuffer() = default;

    CmdBuffer(CmdBuffer &&other) noexcept;
    CmdBuffer(const CmdBuffer &) = delete;

    CmdBuffer & operator=(CmdBuffer &&) = delete;
    CmdBuffer & operator=(const CmdBuffer &) = delete;

private:
    vk::CommandBuffer _buffer;
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_RENDERPASS_CMDBUFFER_HPP
```

Not a whole lot going on - this class really just wraps things nicely for the structure of our code. Here's the implementation:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render/renderpass/CmdBuffer.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void CmdBuffer::allocate(const vk::CommandBufferAllocateInfo &buffer_info) {
    auto result = LogicalDevice::native().allocateCommandBuffers(
        &buffer_info,
        &_buffer
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to llocated command buffer from pool {:#x}: '{}'",
            reinterpret_cast<uint64_t>(VkCommandPool(buffer_info.commandPool)),
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE(
            "Allocated command buffer from pool {:#x}",
            reinterpret_cast<uint64_t>(VkCommandPool(buffer_info.commandPool))
        );
    }
}

// =============================================================================
CmdBuffer::CmdBuffer() :
    _buffer { }
{ }

CmdBuffer::CmdBuffer(CmdBuffer &&other) noexcept :
    _buffer { other._buffer }
{
    other._buffer = nullptr;
}

} // namespace vkl
```

The copy constructor has made an appearance so the forthcoming `CmdPool` object can hold the buffers allocated from it in a `std::vector`. Speaking of `CmdPool`, let's have a look at its declaration.

```cpp
#ifndef VKLEARNIN_RENDER_RENDERPASS_CMDPOOL_HPP
#define VKLEARNIN_RENDER_RENDERPASS_CMDPOOL_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/render/renderpass/CmdBuffer.hpp"

namespace vkl {

class CmdPool final {
public:
    using Buffers = std::vector<CmdBuffer>;
    using BufferIter = Buffers::iterator;

    void create();
    void destroy();

    BufferIter allocate_buffer();

    void reset(const vk::CommandPoolResetFlags flags = { }) const;

    inline auto & native() const { return _pool; }

    CmdPool();
    ~CmdPool() = default;

    CmdPool(CmdPool &&other) noexcept;
    CmdPool(const CmdPool &) = delete;

    CmdPool & operator=(CmdPool &&) = delete;
    CmdPool & operator=(const CmdPool &) = delete;

private:
    vk::CommandPool _pool;
    Buffers _buffers;
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_RENDERPASS_CMDPOOL_HPP
```

Once more, there's a move constructor implemented so `std::vector` can be used to hold onto these objects. The implementation will be:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render/renderpass/CmdPool.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/system/devices/CmdQueue.hpp"

namespace vkl {

// =============================================================================
void CmdPool::create() {
    const vk::CommandPoolCreateInfo pool_info {
        .queueFamilyIndex = LogicalDevice::cmd_queue().index()
    };

    auto result = LogicalDevice::native().createCommandPool(
        &pool_info,
        nullptr,
        &_pool
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to create command pool: '{}'",
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE(
            "Created command pool {:#x}.",
            reinterpret_cast<uint64_t>(VkCommandPool(_pool))
        );
    }
}

// =============================================================================
void CmdPool::destroy() {
    for(auto buffer = _buffers.begin(); buffer != _buffers.end(); ++buffer) {
        LogicalDevice::native().freeCommandBuffers(_pool, buffer->native());
    }

    CONSOLE_TRACE(
        "Destroying command pool {:#x}.",
        reinterpret_cast<uint64_t>(VkCommandPool(_pool))
    );
    LogicalDevice::native().destroyCommandPool(_pool);
}

// =============================================================================
CmdPool::BufferIter CmdPool::allocate_buffer() {
    const vk::CommandBufferAllocateInfo buffer_info {
        .commandPool = _pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1u,
    };

    _buffers.resize(_buffers.size() + 1);
    _buffers.back().allocate(buffer_info);
    return std::prev(_buffers.end());
}

// =============================================================================
void CmdPool::reset(const vk::CommandPoolResetFlags flags) const {
    LogicalDevice::native().resetCommandPool(_pool, flags);
}

// =============================================================================
CmdPool::CmdPool() :
    _pool    { },
    _buffers { }
{ }

CmdPool::CmdPool(CmdPool &&other) noexcept :
    _pool    { other._pool },
    _buffers { std::move(other._buffers) }
{
    other._pool = nullptr;
}

} // namespace vkl
```

In `CmdPool::create()` you can see we're actually still banking on this simply being a graphics queue and that `LogicalDevice` will provide us with a single queue family index that'll permit both graphics and present commands to be submitted.

In the `allocate_buffer()` function, we simply add to `CmdPool`'s internal vector of buffer objects. Vulkan command buffers are meant to be reset (the next function down) rather than freed and/or reallocated when, so there's no mechanism here for freeing individual buffers - the user would have to destroy the entire pool.

Moving on, the `RenderPass` class will be declared as follows:

```cpp
#ifndef VKLEARNIN_RENDER_RENDERPASS_RENDERPASS_HPP
#define VKLEARNIN_RENDER_RENDERPASS_RENDERPASS_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class RenderPass final {
public:
    void create();
    void destroy();

    inline auto native() const { return _render_pass; }

    RenderPass();
    ~RenderPass() = default;

    RenderPass(RenderPass &&) = delete;
    RenderPass(const RenderPass &) = delete;

    RenderPass & operator=(RenderPass &&) = delete;
    RenderPass & operator=(const RenderPass &) = delete;

private:
    std::vector<vk::AttachmentDescription> _attach_descs;
    std::vector<vk::AttachmentReference>   _color_attachments;
    std::vector<vk::SubpassDescription>    _subpasses;
    std::vector<vk::SubpassDependency>     _subpass_deps;

    vk::RenderPass _render_pass;

    void _default_attachments();
    void _default_subpasses();
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_RENDERPASS_RENDERPASS_HPP
```

With a definition looking like this:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render/renderpass/RenderPass.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/render/swapchain/Swapchain.hpp"

namespace vkl {

// =============================================================================
void RenderPass::create() {
    _default_attachments();
    _default_subpasses();

    const vk::RenderPassCreateInfo renderpass_info {
        .attachmentCount = static_cast<uint32_t>(_attach_descs.size()),
        .pAttachments    = _attach_descs.data(),
        .subpassCount    = static_cast<uint32_t>(_subpasses.size()),
        .pSubpasses      = _subpasses.data(),
        .dependencyCount = static_cast<uint32_t>(_subpass_deps.size()),
        .pDependencies   = _subpass_deps.data()
    };

    auto result = LogicalDevice::native().createRenderPass(renderpass_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to create render pass: '{}'",
            to_string(result.result)
        );
    }
    _render_pass = result.value;

    CONSOLE_TRACE(
        "Created Render Pass {:#x}",
        reinterpret_cast<uint64_t>(VkRenderPass(_render_pass))
    );
}

// =============================================================================
void RenderPass::destroy() {
    LogicalDevice::native().destroyRenderPass(_render_pass);
    _render_pass = nullptr;
}

// =============================================================================
void RenderPass::_default_attachments() {
    _attach_descs = {{
        // The image format for the color attachment must match that of the
        // swapchain we're using to present
        .format = Swapchain::image_format(),
        
        // No multisampling... yet
        .samples = vk::SampleCountFlagBits::e1,
        
        // Clear any residual information so we're literally working with a
        // blank canvas
        .loadOp = vk::AttachmentLoadOp::eClear,
        
        // Notify Vulkan that we'd like  to keep whatever we've drawn
        .storeOp = vk::AttachmentStoreOp::eStore,
        
        // Stencil operations aren't useful yet
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        
        // We don't care what layout the image data has when we receive it
        // initially, but once we're done, the image is ready to be presented
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    }};

    _color_attachments = {{
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }};
}

// =============================================================================
void RenderPass::_default_subpasses() {
    _subpasses = {{
        // This subpass is a graphical one
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,

        // ...Which has no input of any kind
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,

        // But does have a single color attachment
        .colorAttachmentCount =
            static_cast<uint32_t>(_color_attachments.size()),
        .pColorAttachments   = _color_attachments.data(),

        // And has no multisampling resolution attachments
        .pResolveAttachments = nullptr,

        // Nor any depth attachments
        .pDepthStencilAttachment = nullptr,

        // As we've only got a single subpass, there's nothing to preserve
        // between subpasses
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = nullptr,
    }};

    _subpass_deps = {{
        // Marking a dependency external indicates it's outside the scope of
        // this render pass. If the source subpass is external, the dependency
        // is everything before this pass. If the destination subpass is
        // external, the dependency is everything after this pass.
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,

        // This subpass will take place after all pipeline stages are complete
        .srcStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe,

        // This subpass will output to the color attachment
        .dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,

        // We want to read from the bottom of the pipe
        .srcAccessMask   = vk::AccessFlagBits::eMemoryRead,

        // We want to write to the color attachment
        .dstAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite,

        // Flagging a subpass dependency by-region means it is local to its
        // framebuffer
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    }};
}

// =============================================================================
RenderPass::RenderPass() :
    _render_pass { }
{ }

} // namespace vkl
```

The last brand new class will be `Framebuffer`.

```cpp
#ifndef VKLEARNIN_RENDER_RENDERPASS_FRAMEBUFFER_HPP
#define VKLEARNIN_RENDER_RENDERPASS_FRAMEBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class RenderPass;

class Framebuffer final {
public:
    void create(const std::vector<vk::ImageView> &attachments,
                const RenderPass &render_pass);
    void destroy();

    inline auto native() const { return _framebuffer; }

    Framebuffer();
    ~Framebuffer() = default;

    Framebuffer(Framebuffer &&other) noexcept;
    Framebuffer(const Framebuffer &) = delete;

    Framebuffer & operator=(Framebuffer &&) = delete;
    Framebuffer & operator=(const Framebuffer &) = delete;

private:
    vk::Framebuffer _framebuffer;
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_RENDERPASS_FRAMEBUFFER_HPP
```

Another move constructor, another hallmark of `std::vector`. The definition goes on to solidify the notion of a framebuffer being an interface between images and render passes:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render/renderpass/Framebuffer.hpp"

#include "vklearnin/render/renderpass/RenderPass.hpp"
#include "vklearnin/render/swapchain/Swapchain.hpp"
#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void Framebuffer::create(const std::vector<vk::ImageView> &attachments,
                         const RenderPass &render_pass)
{
    const vk::FramebufferCreateInfo buffer_info {
        .renderPass      = render_pass.native(),
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .width           = Swapchain::extent().width,
        .height          = Swapchain::extent().height,
        .layers          = 1u,
    };

    auto result = LogicalDevice::native().createFramebuffer(buffer_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to create framebuffer: '{}'",
            to_string(result.result)
        );
    }
    _framebuffer = result.value;

    CONSOLE_TRACE(
        "Created framebuffer {:#x}",
        reinterpret_cast<uint64_t>(VkFramebuffer(_framebuffer))
    );
}

// =============================================================================
void Framebuffer::destroy() {
    CONSOLE_TRACE(
        "Destroying framebuffer {:#x}",
        reinterpret_cast<uint64_t>(VkFramebuffer(_framebuffer))
    );

    LogicalDevice::native().destroyFramebuffer(_framebuffer);

    _framebuffer = vk::Framebuffer();
}

// =============================================================================
Framebuffer::Framebuffer() :
    _framebuffer { }
{ }

Framebuffer::Framebuffer(Framebuffer &&other) noexcept :
    _framebuffer { other._framebuffer }
{
    other._framebuffer = nullptr;
}

} // namespace vkl
```

## Beefing up the Swapchain
Our swapchain from last chapter really doesn't do much, but that was by design. Now we're ready for it to actually start wrangling the images for which it's ostensibly responsible. To that end, we'll add four new member functions and four new accessors to the class.

```cpp
    static void next_image();
    static void reset_fence();

    static void submit(const std::vector<vk::CommandBuffer> &buffers);
    static void present();

    static void create();
    static void destroy();

    inline static auto image_index() { return _draw_index; }
    inline static auto & image(const uint32_t index) { return _images[index]; }

    inline static const auto & extent()       { return _extent; };
    inline static const auto & offset()       { return _offset; };
    inline static const auto & image_format() { return _image_format; }
    inline static auto
    render_area() { return vk::Rect2D { _offset, _extent }; }
```

In order of declaration, let's go over the new functions. First we have `next_image()` which will allow the caller to both request the swapchain update its internal indices as well as block on those updates.

```cpp
void Swapchain::next_image() {
    auto sync = _image_sync[_present_index];

    // Wait for the GPU to signal on the present fence, indicating there's an
    // image for the CPU to schedule work for
    auto result = LogicalDevice::native().waitForFences(
        1u,                // fence count
        &sync.queue_fence, // which fence to wait on
        VK_TRUE,           // require all fences to signal?
        std::numeric_limits<uint64_t>::max() // block effectively forever
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("waitForFences() returned '{}'", to_string(result));
    }

    // Since the fence has been signaled, we can now ask the swapchain which
    // image it'd like us to work with
    result = LogicalDevice::native().acquireNextImageKHR(
        { _swapchain },                      // acquire from yourself
        std::numeric_limits<int64_t>::max(), // block effectively forever
        sync.present_complete,               // which sem to signal
        nullptr,                             // which fence to signal (none)
        &_draw_index                         // resulting image index
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_WARN("acquireNextImageKHR() returned '{}'", to_string(result));
    }
}
```

After calling `next_image()`, the swapchain will probably want to un-signal the command queue fence.

```cpp
void Swapchain::reset_fence() {
    auto result = LogicalDevice::native().resetFences(
        1u,
        &_image_sync[_present_index].queue_fence
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("resetFences() returned '{}'", to_string(result));
    }
}
```

Given the call to `next_image()` has returned, presumably somebody wants to submit some work for the GPU to do; this is done via the swapchain, too.

```cpp
void Swapchain::submit(const std::vector<vk::CommandBuffer> &buffers) {
    // Internally, the command queue needs to know when to begin. In this case,
    // waiting on a completed presentation means there's an image available for
    // writing to.
    vk::Semaphore wait_sems[] = {
        _image_sync[_present_index].present_complete
    };

    // The above semaphore(s) will wait at this stage. In keeping with the idea
    // that these are all graphics commands being submitted, waiting at the
    // very bottom of the pipeline is the right choice.
    vk::PipelineStageFlags wait_dst_stage[] {
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };

    // Some semaphores may also want to know when this particular batch of
    // commands is complete, so they get listed here.
    vk::Semaphore signal_sems[] = {
        _image_sync[_draw_index].draw_complete
    };

    const vk::SubmitInfo submit_info {
        .waitSemaphoreCount = static_cast<uint32_t>(std::size(wait_sems)),
        .pWaitSemaphores = wait_sems,
        .pWaitDstStageMask = wait_dst_stage,
        .commandBufferCount = static_cast<uint32_t>(buffers.size()),
        .pCommandBuffers = buffers.data(),
        .signalSemaphoreCount =
            static_cast<uint32_t>(std::size(signal_sems)),
        .pSignalSemaphores = signal_sems,
    };

    auto result = LogicalDevice::cmd_queue().native().submit(
        submit_info,
        _image_sync[_present_index].queue_fence
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR(
            "Could not submit command buffers: '{}'",
            to_string(result)
        );
    }
}
```

Once all the work is complete, instructing the swapchain to present the image is the next step.

```cpp
void Swapchain::present() {
    // Now that the image being drawn is ready to be presented, these indices
    // will overlap for a touch
    _present_index = _draw_index;

    const vk::PresentInfoKHR present_info {
        // Do make sure the previously submitted work is acctually complete
        .waitSemaphoreCount = 1u,
        .pWaitSemaphores = &_image_sync[_draw_index].draw_complete,

        // With only one surface in this setup, there'll only ever be one
        // swapchain
        .swapchainCount = 1u,
        .pSwapchains = &_swapchain,

        // Again, at the moment both image indices are the same
        .pImageIndices = &_present_index
    };

    auto result = LogicalDevice::cmd_queue().native().presentKHR(present_info);
    if(result == vk::Result::eErrorOutOfDateKHR ||
       result == vk::Result::eSuboptimalKHR)
    {
        CONSOLE_WARN("presentKHR() returned '{}'", to_string(result));
    }
}
```

## Application Demonstrates Itself
`Application` is feeling left out this chapter, so let's give it a new virtual member function.

```cpp
virtual void run_renderpasses(uint32_t frame_index) = 0;
```

Naturally, `Demo` must now implement this, but we're actually going to do a fair bit more than that. First, add these four member variables to `Demo`:

```cpp
    std::vector<vkl::CmdPool>             _cmd_pools;
    std::vector<vkl::CmdPool::BufferIter> _cmd_buffers;
    std::vector<vkl::Framebuffer>         _framebuffers;
    vkl::RenderPass _render_pass;
```

`Demo`'s constructor needs to be updated accordingly:

```cpp
Demo::Demo() :
    _render_pass { }
{
    _cmd_pools.resize(vkl::RenderConfig::image_count);
    _cmd_buffers.resize(vkl::RenderConfig::image_count);
    _framebuffers.resize(vkl::RenderConfig::image_count);
}
```

And now, `Demo::init()` and `Demo::shutdown()` can finally serve a purpose!

```cpp
// =============================================================================
void Demo::init() {
     _render_pass.create();

    for(uint32_t frame = 0; frame < _cmd_pools.size(); ++frame) {
        _cmd_pools[frame].create();
        _cmd_buffers[frame] = _cmd_pools[frame].allocate_buffer();
        _framebuffers[frame].create(
            { vkl::Swapchain::image(frame)->view() },
            _render_pass
        );
    }
}

// =============================================================================
void Demo::shutdown() {
    for(size_t frame = 0; frame < _cmd_pools.size(); ++frame) {
        _cmd_pools[frame].destroy();
        _framebuffers[frame].destroy();
    }

    _render_pass.destroy();
}
```

But we're just getting started. The meat of the update will be in `Demo::run_renderpasses()`, of course.

```cpp
void Demo::run_renderpasses(uint32_t frame_index) {
    _cmd_pools[frame_index].reset();

    auto cmd_buffer = _cmd_buffers[frame_index]->native();

    // The flags in this structure can be used to mark a command buffer as
    // single-use, among other things. In this case, we'll be using a vanilla
    // command buffer.
    const vk::CommandBufferBeginInfo begin_info { };

    // Calling begin tells the command buffer to start recording commands
    auto result = cmd_buffer.begin(begin_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR(
            "Failed to begin command buffer recording: '{}'",
            to_string(result)
        );
    }

    // The clear color specifies what value to fill the swapchain image pixels
    // with when we nuke that area of memory before beginning to draw
    static const vk::ClearValue clear_values[] = {
        { .color { vkl::RenderConfig::CLEAR_COLOR }}
    };

    const vk::RenderPassBeginInfo pass_info {
        .renderPass      = _render_pass.native(),
        .framebuffer     = _framebuffers[frame_index].native(),
        .renderArea      = vkl::Swapchain::render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
        .pClearValues    = clear_values,
    };

    // The command buffer is ready to record commands, and now it knows we
    // mean to record graphics commands in particular. Marking the subpass
    // contents as inline means we're not executing from a secondary command
    // buffer.
    cmd_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

    // ...??? Something graphical will live here, soon.

    // We've issued all of the graphics commands we want, so wrap this render
    // pass up
    cmd_buffer.endRenderPass();

    // And put a bow on the whole thing
    result = cmd_buffer.end();
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR(
            "Failed to end command buffer recording: '{}'",
            to_string(result)
        );
    }

    // Here's some commands, swapchain. See to it that they make their way to
    // the GPU
    vkl::Swapchain::submit({ cmd_buffer });
}
```

There's a variable being used in this function we haven't defined yet. Specifically, `CLEAR_COLOR` has been added to `RenderConfig`. I've made mine a very dark shade of blue.

```cpp
struct RenderConfig final {
    static uint32_t screen_width;
    static uint32_t screen_height;
    static int32_t  screen_x_offset;
    static int32_t  screen_y_offset;

    static uint32_t window_width;
    static uint32_t window_height;
    static int32_t  window_pos_x;
    static int32_t  window_pos_y;

    static uint8_t image_count;
    static bool vsync_on;

    static constexpr std::array<float, 4>
        CLEAR_COLOR { 0.08f, 0.08f, 0.16f, 1.0f };
};
```

In a real application, this color should be something very bright and very obvious ([shocking pink](https://www.reddit.com/r/FF06B5/), anyone?). Because the whole of the swapchain image is generally written to with something useful, if you can see the clear color, it should signal something's gone wrong. But, as we've got nothing else to put on screen at the moment, I went with this near-black-blue of sorts.

## Final Touches
Let's make two more changes. First, set `Engine::render_loop()` up to leverage all this new swapchain functionality:

```cpp
void Engine::render_loop() {
    Timekeeper::frame_start();

        Swapchain::next_image();
        Swapchain::reset_fence();

        _application.run_renderpasses(Swapchain::image_index());
        Swapchain::present();

    Timekeeper::frame_end();
}
```

Second, `Application::run()` will need to look a little different since `Engine` is doing different work.

```cpp
void Application::run() {
    init();

    while(TargetWindow::message_loop()) {
        _engine->render_loop();
        CONSOLE_TRACE("A frame took: {:.06f}", Timekeeper::frametime());
    }

    auto result = LogicalDevice::native().waitIdle();
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to wait for device idle: '{}'",
            to_string(result)
        );
    }

    shutdown();
}
```

With that, your code should now do something we've never seen before: fill the target window with a color!

Not the flashiest graphics demonstration you've ever seen, I'm sure. But we're making great progress and next chapter will see us actually draw our very first triangle. Hot damn!