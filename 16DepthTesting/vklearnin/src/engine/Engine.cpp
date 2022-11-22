#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/Engine.hpp"

#include "vklearnin/system/TargetWindow.hpp"
#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/rendering/Renderer.hpp"
#include "vklearnin/engine/Swapchain.hpp"
#include "vklearnin/engine/Pipeline.hpp"
#include "vklearnin/rendering/Framebuffer.hpp"
#include "vklearnin/rendering/RenderPass.hpp"
#include "vklearnin/engine/FrameData.hpp"
#include "vklearnin/shaders/CameraData.hpp"
#include "vklearnin/shaders/InstanceData.hpp"

////////////////////////////////////////////////////////////////////////////////
// TODO: replace with proper asset management
#include "vklearnin/mesh/XZUnitPlane.hpp"
#include "vklearnin/mesh/UnitCube.hpp"
////////////////////////////////////////////////////////////////////////////////

namespace vkl {

// =============================================================================
void Engine::render_loop() {
    // Advance the swapchain image index, which will block waiting for an image
    // which has been drawn to screen and released
    auto result = _swapchain->next_image(_frame_index);

    // If one of these two hit, it's because the swapchain images are no longer
    // appropriately sized
    if(result == vk::Result::eErrorOutOfDateKHR ||
       result == vk::Result::eSuboptimalKHR)
    {
        CONSOLE_ERROR("Could not get next image on frame {}", _frame_index);
        _image_invalid();
        return;
    }

    // Clear out the frame's command pool
    _frame_index = _swapchain->image_index();
    _frames[_frame_index].cmd_pool().reset();

    // Un-signal the fence controlling this framebuffer; the GPU will signal
    // when it's done again after we submit this buffer's work
    _swapchain->reset_fence();

    // No need for special flags for this application
    vk::CommandBufferBeginInfo begin_info { };

    // Let the command buffer know we're ready to record
    auto &command_buffer = _frames[_frame_index].cmd_buffer().native();
    result = command_buffer.begin(begin_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Failed to begin command buffer recording.");
    }

    // Binding the appropriate pipeline and marking it for drawing commands
    command_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        _pipeline->native()
    );

    // Everybody loves the clear color
    vk::ClearValue clear_values[] = {
        { .color = std::array<float, 4> { 0.08f, 0.08f, 0.16f, 1.0f } },
        { .depthStencil = 1.0f }
    };

    const auto &render_pass = _pipeline->render_pass();

    vk::RenderPassBeginInfo pass_info {
        .renderPass      = render_pass.native(),
        .framebuffer     = render_pass.framebuffer(_frame_index).native(),
        .renderArea      = _swapchain->render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
        .pClearValues    = clear_values,
    };

    static auto start_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::high_resolution_clock::now() - start_time;
    float runtime = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0f;

    auto model_matrix = glm::rotate(
        glm::mat4(1.0f),
        runtime * math::pi_over_four,
        { 0.75f, 1.0f, 0.0f }
    );
    // auto model_matrix = glm::mat4(1.0f);

    _frames[_frame_index].update_instance_data({
        .model_matrix = model_matrix
    });

    // Go time!
    command_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

        // Establish the area we can draw to
        command_buffer.setViewport(0u, _pipeline->viewport());
        command_buffer.setScissor(0u, _pipeline->scissor());

        // Send the view and projection matrices
        command_buffer.pushConstants<CameraData>(
            _pipeline->layout(),
            vk::ShaderStageFlagBits::eVertex,
            0u,
            _camera_data
        );

        command_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _pipeline->layout(),
            0u,
            _frames[_frame_index].descriptor_set().native(),
            { }
        );

        // Renderer::draw(
        //     command_buffer,
        //     _xz_unit_plane->vertex_buffer(),
        //     _xz_unit_plane->index_buffer(),
        //     static_cast<uint32_t>(_xz_unit_plane->indices().size())
        // );
        Renderer::draw(
            command_buffer,
            _unit_cube->vertex_buffer(),
            _unit_cube->index_buffer(),
            static_cast<uint32_t>(_unit_cube->indices().size())
        );

    // With that out of the way, that's this pass handled
    command_buffer.endRenderPass();

    // And the whole of this command buffer, too
    result = command_buffer.end();
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Failed to end command buffer recording.");
    }

    // Give the swapchain back a full command buffer and set it loose
    _swapchain->submit(command_buffer, LogicalDevice::cmd_queue());

    // Swap buffers
    result = _swapchain->present();

    // A present operation can return these two, too. Same approach as above -
    // adjust the required stuff and try again
    if(result == vk::Result::eErrorOutOfDateKHR ||
       result == vk::Result::eSuboptimalKHR)
    {
        CONSOLE_ERROR("Could not present frame {}", _frame_index);
        _image_invalid();
        return;
    }    
    
    _next_frame();
}

// =============================================================================
void Engine::init() {
    VKAllocator::init();

    // Finally on to the meat of Engine's own stuff. First, configure and
    // create a swapchain with the double buffering and surface details we
    // require
    _swapchain = new Swapchain;
    _swapchain->create();

    _create_frames();

    // Next, the pipeline we'll be using needs to load up the shaders modules
    // and configure itself based on the expected inputs
    _pipeline = new Pipeline(*_swapchain);
    _pipeline->vertex_from_binary(
        "../../vklearnin/assets/shaders/05texture_sampler.vert-debug.spv"
    );
    _pipeline->fragment_from_binary(
        "../../vklearnin/assets/shaders/05texture_sampler.frag-debug.spv"
    );
    _pipeline->set_push_constants({{
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .offset = 0u,
        .size = sizeof(CameraData)
    }});
    _pipeline->set_layout({ _frames[0].descriptor_set_layout().native() });
    _pipeline->create();

    _camera_data.proj_matrix = glm::perspective(
        RenderConfig::fov_rad * 0.5f,
        RenderConfig::aspect_ratio,
        0.1f,
        1000.0f
    );

    _camera_data.view_matrix = glm::lookAt(
        { 0.0f, 0.0f, 3.0f },
        math::forward_vec3,
        math::up_vec3
    );

    _xz_unit_plane = new XZUnitPlane;
    _xz_unit_plane->create_buffers();

    _unit_cube = new UnitCube;
    _unit_cube->create_buffers();
}

// =============================================================================
void Engine::shutdown() {
    _xz_unit_plane->destroy_buffers();
    _unit_cube->destroy_buffers();

    _destroy_frames();
    _pipeline->destroy();
    _swapchain->destroy();
}

// =============================================================================
void Engine::_create_frames() {
    _frames.resize(RenderConfig::swapchain_image_count);

    for(uint32_t image_index = 0;
        image_index < RenderConfig::swapchain_image_count;
        ++image_index)
    {
        _frames[image_index].create({
            {
                .binding = 0u,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = 1u,
                .stageFlags = vk::ShaderStageFlagBits::eVertex,
                .pImmutableSamplers = nullptr
            },
            {
                .binding = 1u,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                .descriptorCount = 1u,
                .stageFlags = vk::ShaderStageFlagBits::eFragment,
                .pImmutableSamplers = nullptr
            },
        });
    }

    _frame_index = 0u;

    CONSOLE_TRACE("Created {} frames", _frames.size());
}

// =============================================================================
void Engine::_destroy_frames() {
    for(auto &frame : _frames) {
        frame.destroy();
    }
}

// =============================================================================
void Engine::_image_invalid() {
    // wait for current commands to run their course
    auto result = LogicalDevice::native().waitIdle();
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to wait for idle on image resize.");
    }

    CONSOLE_WARN("Destroy framebuffers");
    _pipeline->destroy_framebuffers();

    CONSOLE_WARN("Destroy swapchain");
    _swapchain->destroy();

    CONSOLE_WARN("Reinitialize window surface");
    TargetWindow::destroy_surface();
    TargetWindow::create_surface();

    CONSOLE_WARN("Recreate swapchain");
    _swapchain->create();

    CONSOLE_WARN("Recreate framebuffers");
    _pipeline->create_framebuffers();

    _pipeline->update_dimensions();

    _camera_data.proj_matrix = glm::perspective(
        RenderConfig::fov_rad * 0.5f,
        RenderConfig::aspect_ratio,
        0.1f,
        1000.0f
    );

    _frame_index = 0u;
}

// =============================================================================
void Engine::_next_frame() {
    // Flipping between zero and one without the use of the mod operator.
    // Courtesy paxdiablo: https://stackoverflow.com/a/4084058/1464937
    _frame_index = 1 - _frame_index;
}

// =============================================================================
Engine::Engine() :
    _swapchain     { nullptr },
    _pipeline      { nullptr },
    _frame_index   { 0u },
    _xz_unit_plane { nullptr },
    _unit_cube     { nullptr }
{ }

Engine::~Engine() {
    delete _swapchain;
    delete _pipeline;
    delete _xz_unit_plane;
    delete _unit_cube;
}

} // namespace vkl