Now that there's a swapchain swapping images, a render pass capturing commands, and a framebuffer interfacing between the two, we're in need of a means by which to organize the drawing itself. This'll be the [pipeline](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPipeline.html), which is comprised of stages. Some of these stages are able (or required) to be directly manipulated by the programmer. This is where the term programmable pipeline (as opposed to the [fixed-function](https://www.khronos.org/opengl/wiki/Fixed_Function_Pipeline) pipeline of yore) comes from.

## Shaders
Shaders are small programs written in a C-like language (in the case of Vulkan, this language can be many different things, but we'll be using [GLSL](https://en.wikipedia.org/wiki/OpenGL_Shading_Language)) that run directly on your graphics card. Like any program, they'll take input, do work on that input, and produce output.

## Pipeline Stages
When discussing OpenGL, the Khronos Group details [several steps](https://www.khronos.org/opengl/wiki/Rendering_Pipeline_Overview) which are taken to draw an image on screen. Below, I'll discuss the steps in a broader sense that will guide our coming work.

- **Vertex Shader** - A [vertex](https://en.wikipedia.org/wiki/Vertex_(geometry)) is a point in space where, for our purposes, two lines meet. Generally, a whole collection of points which describe some shape in space will be fed to the graphics card. An instance of the vertex shader will take one of these points as its input and output its position in context of the image being drawn on screen. A vertex shader can be written to take more inputs and provide more outputs than just position, as well.

- **Tessellation and Geometry Shaders** - These steps are both programmable, both optional, and both can be used to add detail to scene. As they aren't required, we won't dwell on them.

- **Rasterization** - Once the graphics card has all of the points it needs to describe the scene you've built, it can proceed to fill in the (literal) gaps. Using three points to describe a triangle meets the requirements to place the shape in space, but it doesn't tell us what color the triangle might be, for example. Or perhaps the triangle's appearance is comprised of more than one color. The rasterization step collects all of the information necessary to describe what the spaces between the vertices could look like and assembles them into structures which Vulkan and OpenGL calls fragments.

- **Fragment Shader** - Much like the vertex shader, the fragment shader only requires one fragment as input. Each fragment shader then determines a color value for itself, and in the simplest pipelines, this value is assigned to a given pixel on screen.

## SPIR-V and Shader Modules
Vulkan doesn't want to deal with shaders in plain text, so I'm opting to integrate the compilation of any shaders we write into our build system. Specifically, we'll compile shaders into the [SPIR-V](https://www.khronos.org/spir/) binary format and then present them to Vulkan as [shader modules](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkShaderModule.html). To start, we'll have one shader module for the vertex stage and one for the fragment stage of the pipeline.

I've added a new file to `vklearnin/cmake` and called it `CompileShaders.cmake`:

```cmake
# Thanks to diapir for this one. Slightly modified for my own uses:
# https://stackoverflow.com/a/60472877/1464937

find_package(Vulkan REQUIRED)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

cmake_policy(SET CMP0116 NEW)

function(compile_shader target)
    cmake_parse_arguments(
        PARSE_ARGV 1 arg
        ""
        "ENV"
        "SOURCES"
    )
    foreach(source ${arg_SOURCES})
        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            set(output_filename "${source}-debug.spv")
            set(shader_optimization "-O0")
            set(shader_debug "-g")
        elseif(CMAKE_BUILD_TYPE MATCHES "Release")
            set(output_filename "${source}-release.spv")
            set(shader_optimization "-O")
            set(shader_debug "")
        else()
            message(FATAL_ERROR "CMake build type ${CMAKE_BUILD_TYPE} unknown.")
        endif()
        add_custom_command(
            OUTPUT ${output_filename}
            DEPENDS ${source}
            DEPFILE ${source}.d
            COMMAND
                ${glslc_executable}
                $<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
                -mfmt=bin
                -MD -MF ${source}.d
                ${shader_debug}
                -o ${output_filename}
                ${source}
        )

        target_sources(
            ${target} PRIVATE
            ${source}
            ${output_filename}
        )
    endforeach()
endfunction()
```

And to `vklearnin/CMakeLists.txt`, I added the following blocks:

```cmake
file(
    GLOB SHADER_SOURCE
    "${PROJECT_SOURCE_DIR}/assets/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/assets/shaders/*.frag"
)

if("${SHADER_SOURCE}" STREQUAL "")
    message(FATAL_ERROR "Shader source glob was empty")
endif()

compile_shader(
    vklearnin
    ENV vulkan1.2
    SOURCES ${SHADER_SOURCE}
)
```

As the `file()` block indicates, we'll now need to add at least one file ending in `.vert` and one ending in `.frag` to the entirely new directory `assets/shaders/` in order for CMake to even let us continue building. Let's take a look at our first shader, which I've called `01fixed_triangle.vert`:

```c
#version 460 // GLSL version identifier

// Three vertices to describe a triangle in space
const vec4 vertices[3] = vec4[3](
    vec4(-0.5f, -0.5f, 0.0f, 1.0f), // -x, -y is the left corner
    vec4( 0.0f,  0.5f, 0.0f, 1.0f), // zero, +y is the tip of the triangle
    vec4( 0.5f, -0.5f, 0.0f, 1.0f)  // +x, -y is the right corner
);

void main() {
    // The vertex shader only has one required output: the position of the
    // vertex it just completed processing. Normally, there's some more 3D math
    // that goes on before assigning  this value, but this is where we're
    // starting.
	gl_Position = vertices[gl_VertexIndex];
}
```

And the corresponding fragment shader `01fixed_triangle.frag`:

```c
#version 460 // GLSL version identifier

// The zero-th output value of the fragment shader (and the only output this
// stage is required to produced) is the final color of the fragment.
layout (location = 0) out vec4 out_color;

void main() {
	// Just like the fixed vertices of the vertex shader, for simplicity's sake
	// we're going with a fixed fragment color. It seems silly to fix the
	// programmable stages of a pipeline, but we'll come back and spruce these
	// up before long.
	out_color = vec4(0.15f, 0.75f, 0.33f, 1.0f);
}
```

As you may have guessed, we're shooting to see a large, greenish triangle smack in the center of the screen. But these shaders certainly wont run themselves, so let's build out the rest of the required infrastructure.

## The Shader Class
This class will read the SPIR-V binaries the build system created from disk and format the raw  bytes to meet Vulkan's expectations. Here's the declaration:

```cpp
#ifndef VKLEARNIN_RENDER_PIPELINE_SHADER_HPP
#define VKLEARNIN_RENDER_PIPELINE_SHADER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Shader final {
public:
    void create(std::string_view filepath);
    void destroy();

    inline auto native() const { return _shader; }

    Shader();
    ~Shader() = default;

    Shader(Shader &&) = delete;
    Shader(const Shader &) = delete;

    Shader & operator=(Shader &&) = delete;
    Shader & operator=(const Shader &) = delete;

private:
    vk::ShaderModule _shader;

    static std::vector<uint32_t> _spirv_to_binary(std::string_view filepath);
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_PIPELINE_SHADER_HPP
```

And the implementation:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render/pipeline/Shader.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

using Binary = std::vector<uint32_t>;
using String = std::vector<char>;

// =============================================================================
void Shader::create(std::string_view filepath) {
    auto shader_binary = _spirv_to_binary(filepath);
    const vk::ShaderModuleCreateInfo module_info {
        .codeSize = shader_binary.size() * sizeof(uint32_t),
        .pCode = shader_binary.data(),
    };

    auto [result, shader_module] =
        LogicalDevice::native().createShaderModule(module_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create shader module.");
        return;
    }    

    CONSOLE_TRACE(
        "Shader module {:#x} from '{}'",
        reinterpret_cast<uint64_t>(VkShaderModule(shader_module)),
        filepath
    );

    _shader = shader_module;
}

// =============================================================================
void Shader::destroy() {
    CONSOLE_TRACE(
        "Destroying shader module {:#x}",
        reinterpret_cast<uint64_t>(VkShaderModule(_shader))
    );
    LogicalDevice::native().destroyShaderModule(_shader);
    _shader = nullptr;
}

// =============================================================================
Binary Shader::_spirv_to_binary(std::string_view filepath) {
    // Here we're just accounting for the path and filename differences
    // between debug and release builds
    std::filesystem::path shader_path = ASSET_PATH / filepath.data();
    shader_path += SHADER_EXT;

    // Open the SPIR-V binary file and place the "cursor" at the end
    std::ifstream input_file(
        shader_path.native(),
        std::ios::binary | std::ios::ate
    );

    if(!input_file.good()) {
        CONSOLE_CRITICAL("Unable to open binary '{}'", filepath);
        return Binary { };
    }

    // Since we're already at the end, std::ifstream::tellg() will give us the
    // file's size. Capture that, then seek back to the beginning in order to
    // begin reading the file's content properly.
    auto filesize = static_cast<size_t>(input_file.tellg());
    input_file.seekg(0, std::ifstream::beg);

    // Start by reading in an array of characters
    String shader_string(filesize);
    input_file.read(
        shader_string.data(),
        static_cast<std::streamsize>(filesize)
    );
    input_file.close();

    // Then copy the character array into an integer array to provide Vulkan
    // with the binary data in the size and configuration it expects.
    Binary shader_binary(shader_string.size() / sizeof(uint32_t));
    memcpy(shader_binary.data(), shader_string.data(), shader_string.size());

    return shader_binary;
}

// =============================================================================
Shader::Shader() :
    _shader { }
{ }

} // namespace vkl
```

Just like with `RenderConfig::CLEAR_COLOR` from the previous chapter, there are some variables here we haven't defined yet. Specifically, `ASSET_PATH` and `SHADER_EXT`.  I've places these two at  the end of `vklearnin.hpp`.

```cpp
#if VKL_DEBUG
static const std::filesystem::path ASSET_PATH("../../assets");
static const std::string SHADER_EXT("-debug.spv");
#else
static const std::filesystem::path ASSET_PATH("assets");
static const std::string SHADER_EXT("-release.spv");
#endif
```

Now both release and debug builds will be able to find their respective SPIR-V binaries.

## The Pipeline Class
Implementing `Pipeline` will be the bulk of the work for this chapter. Let's dig in.

```cpp
#ifndef VKLEARNIN_RENDER_PIPELINE_PIPELINE_HPP
#define VKLEARNIN_RENDER_PIPELINE_PIPELINE_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/render/pipeline/Shader.hpp"

namespace vkl {

class RenderPass;

class Pipeline final {
public:
    void vert_from_spirv(std::string_view filepath);
    void frag_from_spirv(std::string_view filepath);

    void create(const RenderPass &render_pass);
    void destroy();
    void update_dimensions();

    inline auto native()           const { return _pipeline; }
    inline const auto & viewport() const { return _viewport; }
    inline const auto & scissor()  const { return _scissor;  }

    Pipeline();
    ~Pipeline() = default;

    Pipeline(Pipeline &&) = delete;
    Pipeline(const Pipeline &) = delete;

    Pipeline & operator=(Pipeline &&) = delete;
    Pipeline & operator=(const Pipeline &) = delete;

private:
    Shader _vert;
    Shader _frag;

    std::vector<vk::PipelineShaderStageCreateInfo>     _shader_stages;
    std::vector<vk::PipelineColorBlendAttachmentState> _blend_states;

    vk::Viewport _viewport;
    vk::Rect2D   _scissor;

    vk::PipelineVertexInputStateCreateInfo   _vert_input_info;
    vk::PipelineInputAssemblyStateCreateInfo _assembly_info;
    vk::PipelineViewportStateCreateInfo      _viewport_info;
    vk::PipelineRasterizationStateCreateInfo _raster_info;
    vk::PipelineColorBlendStateCreateInfo    _blend_info;
    std::vector<vk::DynamicState>            _dynamic_states;
    vk::PipelineDynamicStateCreateInfo       _dynamic_state_info;

    vk::PipelineLayout _pipeline_layout;
    vk::Pipeline       _pipeline;

    void _init_input();
    void _init_assembly();
    void _init_viewport();
    void _init_raster();
    void _init_blend();
    void _init_dynamic_states();
    void _init_layout();
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_PIPELINE_PIPELINE_HPP
```

The two shader loading functions feed the provided file path to their respective `Shader`s, then take note that a programmable pipeline stage has been added.

```cpp
// =============================================================================
void Pipeline::vert_from_spirv(std::string_view filepath) {
    _vert.create(filepath);
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = _vert.native(),
        .pName = "main",
    });
}

// =============================================================================
void Pipeline::frag_from_spirv(std::string_view filepath) {
    _frag.create(filepath);
    _shader_stages.emplace_back(vk::PipelineShaderStageCreateInfo {
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = _frag.native(),
        .pName = "main",
    });
}
```

`Pipeline::create()` serves to wrap the many steps required for defining a pipeline up into one place. The `vk::GraphicsPipelineCreateInfo` structure itself will be somewhat sparse here, but  will grow to suit our needs in later chapters.

```cpp
void Pipeline::create(const RenderPass &render_pass) {
    _init_input();
    _init_assembly();
    _init_viewport();
    _init_raster();
    _init_blend();
    _init_dynamic_states();
    _init_layout();

    const vk::GraphicsPipelineCreateInfo pipeline_info {
        .stageCount = static_cast<uint32_t>(_shader_stages.size()),
        .pStages    = _shader_stages.data(),

        .pVertexInputState   = &_vert_input_info,
        .pInputAssemblyState = &_assembly_info,
        .pTessellationState  = nullptr,
        .pViewportState      = &_viewport_info,
        .pRasterizationState = &_raster_info,
        .pMultisampleState   = nullptr,
        .pDepthStencilState  = nullptr,
        .pColorBlendState    = &_blend_info,
        .pDynamicState       = &_dynamic_state_info,

        .layout              = _pipeline_layout,

        // Which render pass will use this pipeline?
        .renderPass          = render_pass.native(),
        // And within that render pass, which subpass will use this pipeline?
        .subpass             = 0u,

        // A new pipeline may be derrived from an existing one, only updating
        // what needs to be updated. The .basePipeline* values designate an
        // existing pipeline to pull from.
        .basePipelineHandle  = nullptr,
        .basePipelineIndex   = 0,
    };

    auto pipeline_return =
        LogicalDevice::native().createGraphicsPipeline({ }, pipeline_info);

    if(pipeline_return.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Unable to create graphics pipelines: '{}'",
            to_string(pipeline_return.result)
        );
        return;
    }

    _pipeline = pipeline_return.value;
    CONSOLE_TRACE(
        "Created graphics pipeline {:#x}",
        reinterpret_cast<uint64_t>(VkPipeline(_pipeline))
    );

}
```

`Pipeline::destroy()` needs to make sure it handles any shader modules created along the way.

```cpp
void Pipeline::destroy() {
    _vert.destroy();
    _frag.destroy();
    
    CONSOLE_TRACE(
        "Destroying graphics pipeline {:#x}, layout {:#x}",
        reinterpret_cast<uint64_t>(VkPipeline(_pipeline)),
        reinterpret_cast<uint64_t>(VkPipelineLayout(_pipeline_layout))
    );

    LogicalDevice::native().destroy(_pipeline_layout);
    LogicalDevice::native().destroy(_pipeline);
}
```

Given that the viewport and scissor for this pipeline are marked as dynamic states, we want to handle updating them host-side.

```cpp
void Pipeline::update_dimensions() {
    auto [width, height] = Swapchain::extent();
    auto [x, y]          = Swapchain::offset();

    _viewport = vk::Viewport {
        .x         =  static_cast<float>(x),
        .y         =  static_cast<float>(height),
        .width     =  static_cast<float>(width),
        .height    = -static_cast<float>(height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    _scissor = vk::Rect2D {
        .offset = { x, y },
        .extent = { width, height },
    };

    CONSOLE_TRACE(
        "Pipeline viewport updated: {:.02f}x{:.02f} ({:.02f}, {:.02f}) ",
        _viewport.width,
        _viewport.height,
        _viewport.x,
        _viewport.y
    );
}
```

The various state initialization functions all have their own minutiae.

```cpp
// =============================================================================
void Pipeline::_init_input() {
    // The vertex shader has no inputs at this point. These initialization
    // values aren't strictly necessary as a result, but I've added them for
    // clarity.
    _vert_input_info = vk::PipelineVertexInputStateCreateInfo {
        .vertexBindingDescriptionCount   = 0u,
        .pVertexBindingDescriptions      = nullptr,
        .vertexAttributeDescriptionCount = 0u,
        .pVertexAttributeDescriptions    = nullptr
    };
}

// =============================================================================
void Pipeline::_init_assembly() {
    // The primitive assembly stage requires knowing how to interpret the
    // vertices you've asked it to draw. Again, we're not feeding anything
    // into the vertex shader, but we do want the assembly stage to see the
    // fixed vertex data as a triangle.
    _assembly_info = {
        // More correctly, the vertices we provided constitute a triangle list
        // with a length of one.
        .topology = vk::PrimitiveTopology::eTriangleList,

        // Restarting the assembly of primitives is not applicable here.
        .primitiveRestartEnable = VK_FALSE
    };
}

// =============================================================================
void Pipeline::_init_viewport() {
    // We need to tell Vulkan how many viewports and scissors we're providing,
    // but since we're going to mark these as dynamic states, the actual
    // pointers should be null.
    _viewport_info = vk::PipelineViewportStateCreateInfo {
        .viewportCount = 1u,
        .pViewports    = nullptr,
        .scissorCount  = 1u,
        .pScissors     = nullptr,
    };

    update_dimensions();
}

// =============================================================================
void Pipeline::_init_raster() {
    _raster_info = {
        // No depth testing is happening yet, so there's nothing to base any
        // clamping on.
        .depthClampEnable = VK_FALSE,

        // There are some situations in which you want a pipeline to complete
        // only the vertex (or geometry, etc) stage on the geometry. In those
        // situations, it's hugely beneficial to discard the primitives before
        // reaching the rasterization stage.
        .rasterizerDiscardEnable = VK_FALSE,

        // The rasterizer can take the points of a polygon and fill them in,
        // only draw their outline, or even just draw the points in space.
        .polygonMode = vk::PolygonMode::eFill,

        // Once the vertex shader has placed a triangle in space, either it's
        // very likely at some oblique angle to the screen. In our case, the
        // fixed vertex data forms a triangle that is perfectly flat on screen,
        // but either way one of the sides of the triangle is not visible. To
        // save on running the fragment shader for every fragment on the side
        // of the triangle facing away from us, we cull the back-facing data.
        .cullMode = vk::CullModeFlagBits::eBack,

        // Which order are the now-processed vertices connected in? Who's on
        // first? This is also called triangle winding, and for us the order
        // is clockwise.
        .frontFace = vk::FrontFace::eClockwise,

        // Once more, there is no depth testing being done, so these values are
        // superfluous.
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,

        // If a line segment is to be reasterized, what width should it be?
        .lineWidth = 1.0f,
    };
}

// =============================================================================
void Pipeline::_init_blend() {
    // Even though blending is disabled, the pipeline still runs a blend stage
    _blend_states = {{
        .blendEnable = VK_FALSE,

        // ...and the blend stage needs to know the color channels to which
        // it's allowed to write
        .colorWriteMask = vk::ColorComponentFlagBits::eR |
                          vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB |
                          vk::ColorComponentFlagBits::eA
    }};

    _blend_info = {
        // The blend stage also needs to know what images it's blending. Since
        // our render pass writes to its color attachemnt, we need to tell
        // the blend stage to look there.
        .attachmentCount = static_cast<uint32_t>(_blend_states.size()),
        .pAttachments    = _blend_states.data()
    };
}

// =============================================================================
void Pipeline::_init_dynamic_states() {
    // Setting the viewport and scissor states to dynamic allows us to change
    // the size of the target surface without recreating the entire pipeline.
    _dynamic_states = { 
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    _dynamic_state_info = {
        .dynamicStateCount = static_cast<uint32_t>(_dynamic_states.size()),
        .pDynamicStates    = _dynamic_states.data(),
    };
}

// =============================================================================
void Pipeline::_init_layout() {
    // Much like the input state above, the layout of this pipeline is empty.
    const vk::PipelineLayoutCreateInfo layout_info {
        .setLayoutCount         = 0u,
        .pSetLayouts            = nullptr,
        .pushConstantRangeCount = 0u,
        .pPushConstantRanges    = nullptr
    };

    // Unlike the input state above, we do have to explicitly create the
    // pipeline layout object.
    auto result = LogicalDevice::native().createPipelineLayout(layout_info);
    if(result.result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not create pipeline layout: '{}'",
            to_string(result.result)
        );
        return;
    }

    CONSOLE_TRACE(
        "Created pipeline layout {:#x}",
        reinterpret_cast<uint64_t>(VkPipelineLayout(result.value))
    );

    _pipeline_layout = result.value;
}
```

## Using the New Classes
Let's change the main loop in `Application::run()` a bit.

```cpp
void Application::run() {
    init();

    float cummulative_frametime = 0.0f;
    uint32_t cummulative_frame_count = 0u;

    while(TargetWindow::message_loop()) {
        _engine->render_loop();

        cummulative_frametime   += Timekeeper::frametime();
        cummulative_frame_count += 1;
        if(cummulative_frametime >= 0.5f) {
            CONSOLE_TRACE(
                "{:.02f} avg fps",
                cummulative_frame_count / cummulative_frametime
            );
            cummulative_frametime = 0.0f;
            cummulative_frame_count = 0u;
        }
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

This'll make our terminal output enormously less verbose, but also offer a more familiar metric that is still entirely without meaning at the moment. =)

Over in `Demo`, add a `vkl::Pipeline`-type member variable and update `init()`/`shutdown()` accordingly.

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

    _pipeline.vert_from_spirv("shaders/01fixed_triangle.vert");
    _pipeline.frag_from_spirv("shaders/01fixed_triangle.frag");
    _pipeline.create(_render_pass);
}

// =============================================================================
void Demo::shutdown() {
    _pipeline.destroy();

    for(size_t frame = 0; frame < _cmd_pools.size(); ++frame) {
        _cmd_pools[frame].destroy();
        _framebuffers[frame].destroy();
    }

    _render_pass.destroy();
}
```

And finally, we now need to work with our new pipeline in `Demo::run_renderpasses()`:

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

        // Per the fixed data in the vertex shader, we want to draw three
        // vertices once. The third and fourth parameters will only be non-zero
        // if there's some extra data amidst the vertices.
        cmd_buffer.draw(
            3u,     // vertex count
            1u,     // instance count
            0u,     // first vertex
            0u      // first instance
        );

    // We've issued all of the graphics commands we want, so wrap this render
    // pass up
    cmd_buffer.endRenderPass();
```