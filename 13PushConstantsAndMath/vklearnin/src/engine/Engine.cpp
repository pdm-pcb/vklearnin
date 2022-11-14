#include "vklearnin/vklearnin.hpp"
#include "vklearnin/engine/Engine.hpp"

#include "vklearnin/system/TargetWindow.hpp"
#include "vklearnin/rendering/LogicalDevice.hpp"
#include "vklearnin/rendering/DeviceQueue.hpp"
#include "vklearnin/engine/Swapchain.hpp"
#include "vklearnin/engine/Pipeline.hpp"
#include "vklearnin/rendering/Renderer.hpp"

////////////////////////////////////////////////////////////////////////////////
// TODO: replace with proper asset management
#include "vklearnin/mesh/XZPlane.hpp"
////////////////////////////////////////////////////////////////////////////////

namespace vkl {

// =============================================================================
void Engine::render_loop() {
    // flip between zero and one, without a mod operation
    // courtesy paxdiablo: https://stackoverflow.com/a/4084058/1464937
    _current_framebuffer = 1 - _current_framebuffer;

    auto result = _swapchain->next_image_index(_current_framebuffer);
    if(result != vk::Result::eSuccess) {
        _image_invalid();
        return;
    }

    _swapchain->reset_fences(_current_framebuffer);

    const auto image_index = _swapchain->current_image_index();
    const auto &graphics_queue = LogicalDevice::graphics_queue();
    graphics_queue.reset_cmd_pool(image_index);

    vk::CommandBufferBeginInfo begin_info { };

    auto &command_buffer = graphics_queue.cmd_buffer(image_index);
    result = command_buffer.begin(begin_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Failed to begin command buffer recording.");
    }

    // bind the pipeline so everything's current
    command_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        _pipeline->native()
    );

    // initial setup for the pass
    vk::ClearValue clear_values[] = {
        { .color { std::array<float, 4> { 0.01f, 0.01f, 0.02f, 1.0f }}}
    };

    vk::RenderPassBeginInfo pass_info {
        .renderPass      = _pipeline->renderpass(),
        .framebuffer     = _framebuffers[_current_framebuffer].native(),
        .renderArea      = _swapchain->render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
        .pClearValues    = clear_values,
    };

    // go time!
    command_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

        // set the dynamic traits of the pipeline
        command_buffer.setViewport(0u, _pipeline->viewport());
        command_buffer.setScissor(0u, _pipeline->scissor());

        command_buffer.pushConstants<CameraData>(
            _pipeline->layout(),
            vk::ShaderStageFlagBits::eVertex,
            0u,
            _camera_data
        );

        Renderer::draw(
            command_buffer,
            _xzplane->vertex_buffer(),
            _xzplane->index_buffer(),
            static_cast<uint32_t>(_xzplane->indices().size())
        );

    command_buffer.endRenderPass();
    result = command_buffer.end();
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Failed to end command buffer recording.");
    }

    _swapchain->submit(command_buffer, graphics_queue,
                        _current_framebuffer);

    result = _swapchain->present(_current_framebuffer);
    if(result != vk::Result::eSuccess) {
        _image_invalid();
    }
}

// =============================================================================
void Engine::init() {
    CONSOLE_TRACE("");

    // Finally on to the meat of Engine's own stuff. First, configure and
    // create a swapchain with the double buffering and surface details we
    // require
    _swapchain = new Swapchain;
    _swapchain->create();

    // Next, the pipeline we'll be using needs to load up the shaders modules
    // and configure itself based on the expected inputs
    _pipeline = new Pipeline(*_swapchain);
    _pipeline->vertex_from_binary(
        "../../vklearnin/assets/shaders/03push_constant.vert-debug.spv"
    );
    _pipeline->fragment_from_binary(
        "../../vklearnin/assets/shaders/01fixed_color.frag-debug.spv"
    );
    _pipeline->init_layout();
    _pipeline->init_render_passes();
    _pipeline->create();

    // Framebuffers serve to tie the swapchain and the pipeline together
    _create_framebuffers();

    _camera_data.proj_matrix = glm::perspective(
        RenderConfig::fov_rad * 0.5f,
        RenderConfig::aspect_ratio,
        0.1f,
        1000.0f
    );

    _camera_data.view_matrix = glm::lookAt(
        { 0.0f, 0.0f, 2.0f },
        math::forward_vec3,
        math::up_vec3
    );

    _xzplane = new XZPlane;
    _xzplane->create_buffers();
}

// =============================================================================
void Engine::shutdown() {
    CONSOLE_TRACE("");

    _xzplane->destroy_buffers();

    _destroy_framebuffers();
    _pipeline->destroy();
    _swapchain->destroy();
}

// =============================================================================
void Engine::_create_framebuffers() {
    _framebuffers.resize(RenderConfig::swapchain_image_count);

    for(uint32_t image_index = 0;
        image_index < RenderConfig::swapchain_image_count;
        ++image_index)
    {
        auto &framebuffer = _framebuffers[image_index];
        framebuffer.create(*_swapchain, *_pipeline, image_index);
    }

    _current_framebuffer = 1u;

    CONSOLE_TRACE("Created {} framebuffers", _framebuffers.size());
}

// =============================================================================
void Engine::_destroy_framebuffers() {
    CONSOLE_TRACE("Destroy framebuffers");
    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }
}

// =============================================================================
void Engine::_image_invalid() {
    // wait for current commands to run their course
    auto result = LogicalDevice::native().waitIdle();
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to wait for idle on image resize.");
    }

    _destroy_framebuffers();
    _current_framebuffer = 1u;

    CONSOLE_WARN("Destroy swapchain");
    _swapchain->destroy();

    CONSOLE_WARN("Reinitialize window surface");
    TargetWindow::destroy_surface();
    TargetWindow::create_surface();

    CONSOLE_WARN("Recreate swapchain");
    _swapchain->create();

    _create_framebuffers();

    _pipeline->update_dimensions();

    _camera_data.proj_matrix = glm::perspective(
        RenderConfig::fov_rad * 0.5f,
        RenderConfig::aspect_ratio,
        0.1f,
        1000.0f
    );
}

// =============================================================================
Engine::Engine() :
    _swapchain { nullptr },
    _pipeline  { nullptr },
    _xzplane   { nullptr }
{ }

Engine::~Engine() {
    delete _swapchain;
    delete _pipeline;
    delete _xzplane;
}

} // namespace vkl