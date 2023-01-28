The silliest part of the last chapter is that our programmable pipeline is fixed. The vertices are fixed, the color is fixed, the whole thing just bores the poor GPU to tears. But take heart; we'll fix some of that this chapter.

## Vulkan Buffers
The term buffer is in the running for the most overused term in computer science. None the less, the term is apt to describe what Vulkan calls a buffer. A [buffer](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#resources-buffers) is a handle for some contiguous area of memory, whether it's local to the CPU or the GPU. They're usually read as literal values or data structures, such as vertex data. One might also apply a [buffer view](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkBufferView.html) to support executing image operations on the buffer or some part of it, but that's for another time.

This first stab at buffers will serve the exclusive purpose of a vertex buffer. All we want to do is be able to provide vertex data to the shader from the host application, rather than hard coding it. What's fun about this is that shaders can accept more than just position data in a per-vertex manner. So let's start by defining the idea of a `Vertex.`

```cpp
#ifndef VKLEARNIN_MESHES_VERTEX_HPP
#define VKLEARNIN_MESHES_VERTEX_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Vertex {
public:
    using BindingDescriptions = std::vector<vk::VertexInputBindingDescription>;
    using AttribDescriptions = std::vector<vk::VertexInputAttributeDescription>;

    static inline const auto & binding_desc() { return _binding_desc; }
    static inline const auto & attrib_desc()  { return _attrib_desc;  }

    Vertex(const std::array<float, 4> &position,
           const std::array<float, 4> &color);
    Vertex() = delete;

private:
    static const BindingDescriptions _binding_desc;
    static const AttribDescriptions  _attrib_desc;

    std::array<float, 4> _position;
    std::array<float, 4> _color;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_VERTEX_HPP
```

If you ignore the long Vulkan data types for a moment, you can see that what I'm calling a vertex is going to be four `float`s to describe a point in 3D space, and four  `float`s to describe a four-channel color. The other two `vk::*Description` variables are going to help describe this type of vertex to the pipeline later.

What should `BindingDescriptions` look like, then?

```cpp
// The input binding description tells a pipeline that...
const Vertex::BindingDescriptions Vertex::_binding_desc {{
    // These data will be bound to the zeroth slot
    .binding   = 0u,

    // The space between the beginning of one vertex's worth of information
    // and the beginning of the next will be the size of this class, or
    // 2 * 4 * sizeof(float)
    .stride    = sizeof(Vertex),

    // And that the data blocks described by the above stride size are going to
    // be per-vertex in nature, as opposed to per-instance if we were drawing
    // several copies of the same geometry
    .inputRate = vk::VertexInputRate::eVertex
}};
```

And the `AttribDescriptions`:

```cpp
// While the binding description tells a pipeline where to find data that often
// repeat (one copy for each vertex, in our case) the attribute description
// provides clarification for each member of those repeating blocks
const Vertex::AttribDescriptions Vertex::_attrib_desc {
    {
        // Location here corresponds to the location specified at the top of
        // our shader's source code
        .location = 0u,

        // This binding matches the one above
        .binding  = 0u,

        // Because I'm using four floats to describe both position and color,
        // the format specifies 32 bits for each. In the case of vk::Format,
        // the ideas of RGBA and XYZW are interchangable.
        .format   = vk::Format::eR32G32B32A32Sfloat,

        // This usage of the offsetof macro will resolve to zero, but for color
        // below, it'll provide an offset of 16 bytes - the same as four floats.
        .offset   = static_cast<uint32_t>(offsetof(Vertex, _position)),
    },
    {
        .location = 1u,
        .binding  = 0u,
        .format   = vk::Format::eR32G32B32A32Sfloat,
        .offset   = static_cast<uint32_t>(offsetof(Vertex, _color)),
    }
};
```

This information entirely describes the vertex data we'll feed to the shader. But now we need a physical location in memory to provide this well-described data to. Enter the `VertexBuffer` class.

```cpp
#ifndef VKLEARNIN_MESHES_VERTEXBUFFER_HPP
#define VKLEARNIN_MESHES_VERTEXBUFFER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/Vertex.hpp"

namespace vkl {

class VertexBuffer {
public:
    void create(const size_t size_bytes,
                const vk::BufferUsageFlags usage_flags,
                const vk::SharingMode sharing_mod,
                const vk::MemoryPropertyFlags memory_properties);
    void destroy();

    void populate_buffer(const std::vector<Vertex> &vertices);

    inline auto native() const { return _buffer; }

    VertexBuffer();
    ~VertexBuffer() = default;

    VertexBuffer(VertexBuffer &&) = delete;
    VertexBuffer(const VertexBuffer &) = delete;

    VertexBuffer & operator=(VertexBuffer &&) = delete;
    VertexBuffer & operator=(const VertexBuffer &) = delete;

private:
    size_t           _size;
    vk::Buffer       _buffer;
    vk::DeviceMemory _memory;

    void _allocate(const vk::MemoryPropertyFlags memory_properties);

    static uint32_t _find_memory_type(const vk::MemoryPropertyFlags flags,
                                      const vk::MemoryRequirements &reqs);
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_VERTEXBUFFER_HPP
```

There's a lot of talk about memory properties and requirements here, so let's turn to the implementation for some clarity. `VertexBuffer::create()` looks like you might expect, populating a create info structure and asking the logical device to make something for us.

```cpp
void VertexBuffer::create(const size_t size_bytes,
                          const vk::BufferUsageFlags usage_flags,
                          const vk::SharingMode sharing_mode,
                          const vk::MemoryPropertyFlags memory_properties)
{
    const vk::BufferCreateInfo buffer_info {
        .size        = size_bytes,
        .usage       = usage_flags,
        .sharingMode = sharing_mode,

        // Why should a generic block of memory care about device queue
        // families, you might ask? Provided this structure assures Vulkan we
        // do not intend to share this buffer between threads, the below are
        // simply ignored.
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices   = nullptr,
    };

    auto result = LogicalDevice::native().createBuffer(buffer_info);
    if(result.result != vk::Result::eSuccess || !result.value) {
        CONSOLE_CRITICAL(
            "Failed to create {}-byte vertex buffer: '{}'",
            size_bytes,
            to_string(result.result)
        );
        return;
    }

    CONSOLE_TRACE(
        "Created vertex buffer {:#x}",
        reinterpret_cast<uint64_t>(VkBuffer(result.value))
    );

    _buffer = result.value;
    _size = size_bytes;

    _allocate(memory_properties);
}
```

But the last line seems out of place at first blush. Shouldn't "creating" a buffer also allocate it? Not necessarily, as the `vk::Buffer` object is just a handle. The logical device has given us assurances that we will be able to allocate some memory conforming to our requested size and usage flags. The rest is up to us. So, on to `_allocate()`.

```cpp
void VertexBuffer::_allocate(const vk::MemoryPropertyFlags memory_properties) {
    // The first order of business is to query the logical device about what
    // available memory matches properties we've specified thus far. A zero-
    // initialized vk::MemoryRequirements structure indicates that the
    // allocation we're after is the whole size of the buffer we've already
    // described with no offset.
    vk::MemoryRequirements mem_reqs { };
    LogicalDevice::native().getBufferMemoryRequirements(_buffer, &mem_reqs);

    // This function call will check the joint requirements of ourselves and
    // the logical device against the types of memory offered by the physical
    // device.
    auto type_index = _find_memory_type(memory_properties, mem_reqs);

    // Once a suitable memory type (and its index) is located, we're ready to
    // actually allocate the buffer.
    const vk::MemoryAllocateInfo alloc_info {
        .allocationSize = _size,
        .memoryTypeIndex = type_index,
    };

    auto alloc_result = LogicalDevice::native().allocateMemory(alloc_info);
    if(alloc_result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to allocate {} bytes for vertex buffer {:#x}: '{}'",
            _size,
            reinterpret_cast<uint64_t>(VkBuffer(_buffer)),
            to_string(alloc_result.result)
        );
        return;
    }

    CONSOLE_TRACE(
        "\n\tAllocated {} bytes : {:#x}"
        "\n\tFor buffer {:#x}",
        _size,
        reinterpret_cast<uint64_t>(VkDeviceMemory(alloc_result.value)),
        reinterpret_cast<uint64_t>(VkBuffer(_buffer))
    );

    _memory = alloc_result.value;

    // Finally, 
    auto bind_result = LogicalDevice::native().bindBufferMemory(
        _buffer,
        _memory,
        0u
    );

    if(bind_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Binding attempt failed with '{}' for:"
            "\n\tVertex Buffer: {:#x}"
            "\n\tDevice Memory: {:#x}",
            to_string(bind_result),
            reinterpret_cast<uint64_t>(VkBuffer(_buffer)),
            reinterpret_cast<uint64_t>(VkDeviceMemory(_memory))
        );
    }
}
```

The details of how `_find_memory_type()` finds a suitable type index for us go like this:

```cpp
uint32_t VertexBuffer::_find_memory_type(const vk::MemoryPropertyFlags flags,
                                         const vk::MemoryRequirements &reqs)
{
    const auto &memory_properties = PhysicalDevice::memory_props();
    const auto type_count = memory_properties.memoryTypeCount;

    // This bit-rithmetic bears some explanation. We're checking two bit fields
    // against our requirements for the memory itself.
    for(uint32_t type_index = 0u; type_index < type_count; ++type_index) {

        // Each type index is actually a field in memoryTypeBits. If the index
        // we're currently on is enabled, that means we've found a matching
        // memory type.
        if((reqs.memoryTypeBits & (1u << type_index)) != 0u) {
            const auto &props = memory_properties.memoryTypes[type_index];

            // The second check is against the memory properties. This can be
            // any combination of local to the CPU, local to the GPU, visible
            // to the CPU or not, and more.
            if(props.propertyFlags & flags) {
                return type_index;
            }
        }
    }

    CONSOLE_CRITICAL("Could not find memory to match buffer requirements.");
    return std::numeric_limits<uint32_t>::max();
}
```

`VertexBuffer` has one more capability that needs to be detailed - the user can fill it with data.

```cpp
void VertexBuffer::populate_buffer(const std::vector<Vertex> &vertices) {    
    // Asking the logical device to map a given memory handle provides us with
    // a destination for the memcpy() below
    auto result = LogicalDevice::native().mapMemory(_memory, 0u, _size);

    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to map device memory {:#x}: '{}'",
            reinterpret_cast<uint64_t>(VkDeviceMemory(_memory)),
            to_string(result.result)
        );
        return;
    }

    memcpy(result.value, vertices.data(), _size);
    LogicalDevice::native().unmapMemory(_memory);
}
```

## New Shaders
The above changes will be lost on the shader code we've already written.  Here's the vertex shader we'll want to use.

```c
#version 460 core

// While the descriptions in the Vertex class specify a binding of zero along
// with a location of zero, we only need to specify a location here. Being
// explicit about both the binding and location will come with later
// improvements to our shader code.
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 in_color;

// We're now specifying an optional output of the vertex shader. Since the
// vertex data is providing a color for us, we want to pass it along to
// the fragment shader.
layout(location = 0) out vec4 out_color;

void main() {
	out_color   = in_color;
	gl_Position = position;
}
```

And the corresponding fragment shader.

```c
#version 460 core

// Here is the matching location and data type for the color output value
// provided by the vertex shader.
layout(location = 0) in vec4 in_color;

// And the same required output color from last time, but with out the fixed
// value.
layout(location = 0) out vec4 out_color;

// Both shader stages are still just passthroughs that do no meaningful work
// to their respective data, but when you combine them into a pipeline, some
// cool stuff happens.
void main() {
    out_color = in_color;
}
```

## Our First Mesh
We've defined a vertex as something that describes both position and color. We've also laid the scaffolding to jump through Vulkan's memory hoops. Now it's time to leverage the connection to the pipeline a vertex buffer affords us.

I'm going to call our first mesh `XYPlane`. You might've guessed, but this will describe a plane sitting on the X and Y axes of our target window's render surface. Here's the declaration:

```cpp
#ifndef VKLEARNIN_MESHES_XYPLANE_HPP
#define VKLEARNIN_MESHES_XYPLANE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/meshes/VertexBuffer.hpp"

namespace vkl {

class XYPlane {
public:
    using CornerColors = std::array<std::array<float, 4>, 4>;

    void init(const float scale, const CornerColors corner_colors);
    void shutdown();

    inline const auto & vertex_buffer() const { return _vertex_buffer; }
    inline auto vertex_count() const { return _vertex_data.size(); }

    XYPlane();
    ~XYPlane() = default;

    XYPlane(XYPlane &&) = delete;
    XYPlane(const XYPlane &) = delete;

    XYPlane & operator=(XYPlane &&) = delete;
    XYPlane & operator=(const XYPlane &) = delete;

private:
    std::vector<Vertex> _vertex_data;
    VertexBuffer        _vertex_buffer;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_XYPLANE_HPP
```

Other than the `using` statement near the top, this class is a simple one. It will contain the vertex buffer and data used in that buffer, and that's it. The `init()` function is the only interesting thing here.

```cpp
void XYPlane::init(const float scale, const CornerColors corner_colors) {
    // If you were to sketch these values out on a traditional Cartesian plane,
    // you'd get two overlapping triangles. Graphics hardware works first and
    // last in triangles, so a plane is defined by six points, rather than four.
    
    // Using the scale value here allows us to alter the size of the plane on
    // screen from the client code in Demo. I've chosen to insit on uniform
    // scaling for all points, so the resulting geometry will always be a plane.

    // Finally, the CornerColors only has four values so that the repeated
    // vertices between our two triangles match in appearance as well as
    // location.
    _vertex_data = {
        {{ -scale, -scale, 0.0f, 1.0f }, corner_colors[0] },
        {{ -scale,  scale, 0.0f, 1.0f }, corner_colors[1] },
        {{  scale,  scale, 0.0f, 1.0f }, corner_colors[2] },

        {{ -scale, -scale, 0.0f, 1.0f }, corner_colors[0] },
        {{  scale,  scale, 0.0f, 1.0f }, corner_colors[2] },
        {{  scale, -scale, 0.0f, 1.0f }, corner_colors[3] },
    };

    _vertex_buffer.create(
        // The vertex buffer will match the size of the vertex data exactly.
        sizeof(Vertex) * _vertex_data.size(),

        // We only want this memory to be used as a vertex buffer.
        vk::BufferUsageFlagBits::eVertexBuffer,

        // Exclusive mode means this buffer will not be shared between threads.
        vk::SharingMode::eExclusive,

        // Host visible and coherent describes memory the CPU is able to write
        // to in a predictable way.
        vk::MemoryPropertyFlagBits::eHostVisible |
        vk::MemoryPropertyFlagBits::eHostCoherent
    );

    // Now the buffer is ready to receive the data we specified above
    _vertex_buffer.populate_buffer(_vertex_data);
}
```

## Changes to Pipeline
`Pipeline` is going to receive one new public member function:

```cpp
    void describe_vertex_input(
        const Vertex::BindingDescriptions &bindings,
        const Vertex::AttribDescriptions &attributes
    );
```

This will effectively replace `Pipeline::_init_input()`. Once there's user-provided vertex data, there must also be user-provided descriptions of that data.

```cpp
void Pipeline::describe_vertex_input(
    const Vertex::BindingDescriptions &bindings,
    const Vertex::AttribDescriptions &attributes)
{
    auto binding_count = static_cast<uint32_t>(bindings.size());
    auto attrib_count  = static_cast<uint32_t>(attributes.size());

    // The caller of this function will provide the details of the vertex
    // data this pipeline will expect to see in any given vertex buffer.

    _vert_input_info = {
        .vertexBindingDescriptionCount = binding_count,
        .pVertexBindingDescriptions    = bindings.data(),

        .vertexAttributeDescriptionCount = attrib_count,
        .pVertexAttributeDescriptions   = attributes.data(),
    };
}
```

## Using XYPlane in Demo
We're finally ready for the user code to deploy these new features. Give `Demo` a new `XYPlane` member, and let's freshen up `Demo::init()` to reflect that:

```cpp
// =============================================================================
void Demo::init() {
     _render_pass.create();

    for(uint32_t frame = 0; frame < _framebuffers.size(); ++frame) {
        _framebuffers[frame].create(
            { vkl::Swapchain::image(frame)->view() },
            _render_pass
        );
    }

    // Don't forget to load the new shaders we just wrote
    _pipeline.vert_from_spirv("shaders/02flat_color.vert");
    _pipeline.frag_from_spirv("shaders/02flat_color.frag");

    // The pipeline now expects us to describe the vertex input data we'll
    // provide
    _pipeline.describe_vertex_input(
        vkl::Vertex::binding_desc(),
        vkl::Vertex::attrib_desc()
    );

    _pipeline.create(_render_pass);

    // Initalize the XYPlane object with a suitable scale and color values
    _xy_plane.init(
        // Use a scale of 1.0f if you want the plane to fill the entire
        // rendering surface. Using 0.5f allows us to still see the clear color
        // and validate that the plane is shaped and positioned like we expect.
        0.5f,
        {{
            { 1.0f, 0.0f, 0.0f, 1.0f }, // Red
            { 0.0f, 1.0f, 0.0f, 1.0f }, // Green
            { 0.0f, 0.0f, 1.0f, 1.0f }, // Blue
            { 1.0f, 1.0f, 1.0f, 1.0f }, // White
        }}
    );
}
```

And in `run_renderpasses()`, there will need to be extra commands recorded to the command buffer.

```cpp
    // The command buffer is ready to record commands, and now it knows we
    // mean to record graphics commands in particular. Marking the subpass
    // contents as inline means we're not executing from a secondary command
    // buffer.
    cmd_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

        // The first step in executing any pipeline is to bind it
        cmd_buffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            _pipeline.native()
        );

        // As the above pipeline has a dynamic viewport and scissor state,
        // these need to be specified after binding.
        cmd_buffer.setViewport(0u, _pipeline.viewport());
        cmd_buffer.setScissor(0u, _pipeline.scissor());

        // The command buffer allows us to bind multiple vertex buffers in a
        // single go, which saves on time when drawing more than one object.
        // For now, populating these two std::vectors is just a formality.
        const std::vector<vk::Buffer> vertex_buffers {
            _xy_plane.vertex_buffer().native()
        };

        const std::vector<vk::DeviceSize> offsets {
            0u
        };

        // Bind the XYPlane's vertex buffer
        cmd_buffer.bindVertexBuffers(
            0u,
            vertex_buffers,
            offsets
        );

        // This is the same draw command as before, but because we've bound a
        // vertex buffer, the mesh data in XYPlane is now available to the
        // shaders.
        cmd_buffer.draw(
            static_cast<uint32_t>(_xy_plane.vertex_count()), // total vertices
            static_cast<uint32_t>(vertex_buffers.size()),    // total instances
            0u, // first vertex
            0u  // first instance
        );

    // We've issued all of the graphics commands we want, so wrap this render
    // pass up
    cmd_buffer.endRenderPass();
```

If you compile and run this code, you should see an interesting result. I get this:

![[Pasted image 20230122002502.png]]

Two things stand out here. First - we only specified four colors but got arguably infinitely more than that, creating this interesting gradient effect. This is because after the vertex shader has run, that vertex knows where it is in space. When a given fragment shader runs, it knows where _it_ is in space, as well. Given that we're providing color data for each vertex, the pipeline interpolates between each vertex when deciding what color a given fragment should be, based on where that fragment is. This is of course pretty all on its own, but the fact that the pipeline interpolates values like this automatically will also come in very handy in the future.

The second trait of this image is something you might've noticed in the last chapter with our fixed triangle. The vertex positions we actually specified for `XYPlane` describe a square, not a rectangle. If you've got exactly the same code I do, and you're running it on a monitor with a 16:9 aspect ratio, you'll see something like the screenshot above. If you're running on an ultrawide display, it'll look even sillier. Only if both your monitor and your monitor's pixels are perfectly square will you see a rainbow square like the values we provided imply. This issue comes down to how the math of 3D graphics works, and we'll address it soon.

Looking behind the actual render surface shows some more problems, though. This is what I see on my machine:

![[Pasted image 20230122003631.png]]

Ruh roh. `VKDebugger` has warned us about something! It looks like allocating 192 bytes (two arrays of four `float`s each for a vertex, and six vertices total) isn't what Vulkan expects us to do. Indeed, memory management in Vulkan is a [whole](https://developer.nvidia.com/vulkan-memory-management) [big](https://gpuopen.com/vulkan-memory-allocator/) [thing](https://asawicki.info/news_1740_vulkan_memory_types_on_pc_and_how_to_use_them) all to itself. As with the stretchy square above, we'll address it soon.

Even despite these curiosities, I count this as a resounding success. You've successfully sent information to a whole separate computer within your computer, asked it to do a little work with the information, and now you can see the result of that work. This is precisely what I dig about graphics programming, so if you're happy to see a rainbow box on your screen, let's keep going!