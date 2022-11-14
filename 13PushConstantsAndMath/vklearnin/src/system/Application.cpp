#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/Application.hpp"

#include "vklearnin/rendering/GraphicsInstance.hpp"
#include "vklearnin/system/Win32TargetWindow.hpp"
#include "vklearnin/rendering/LogicalDevice.hpp"
#include "vklearnin/rendering/DeviceQueue.hpp"
#include "vklearnin/rendering/Swapchain.hpp"
#include "vklearnin/rendering/Pipeline.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/mesh/XZPlane.hpp"

namespace vkl {

#if defined(__linux__)
    using TargetWindow = XCBTargetWindow;
#elif defined(_WIN32)
    using TargetWindow = Win32TargetWindow;
#endif

// =============================================================================
void Application::init() {
    GraphicsInstance::init();

    TargetWindow::spawn_window();
    TargetWindow::init_surface();

    GraphicsInstance::init_devices();

    _swapchain = new Swapchain;
    _swapchain->create();

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

    _framebuffers.reserve(RenderConfig::swapchain_image_count);
    for(uint32_t image_idx = 0;
        image_idx < RenderConfig::swapchain_image_count;
        ++image_idx)
    {
        _framebuffers.emplace_back();
    }

    // Tie the framebuffers to the swapchain images
    for(uint32_t image_idx = 0;
        image_idx < RenderConfig::swapchain_image_count;
        ++image_idx)
    {
        auto &framebuffer = _framebuffers[image_idx];
        framebuffer.create(_swapchain->extent(),
                           _swapchain->image_view(image_idx),
                           _pipeline->renderpass());
    }

    _current_framebuffer = 1u;
    _running = true;
}

// =============================================================================
void Application::run() {
    XZPlane xzplane;
    xzplane.create_buffers();

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

    // float runtime = 0.0f;

    while(_running) {
        // flip between zero and one, without a mod operation
        // courtesy paxdiablo: https://stackoverflow.com/a/4084058/1464937
        _current_framebuffer = 1 - _current_framebuffer;

        auto result = _swapchain->next_image_index(_current_framebuffer);
        if(result != vk::Result::eSuccess) {
            _image_invalid();
            continue;
        }

        auto begin = std::chrono::high_resolution_clock::now();
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
                xzplane.vertex_buffer(),
                xzplane.index_buffer(),
                static_cast<uint32_t>(xzplane.indices().size())
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

        _running = TargetWindow::message_loop();
    }

    // wait for current commands to run their course
    auto result = LogicalDevice::native().waitIdle();
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to wait for idle on image resize.");
    }

    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }

    _swapchain->destroy();

    xzplane.destroy_buffers();
}

// =============================================================================
void Application::_image_invalid() {
    // wait for current commands to run their course
    auto result = LogicalDevice::native().waitIdle();
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to wait for idle on image resize.");
    }

    CONSOLE_WARN("Destroy framebuffers");
    for(auto &framebuffer : _framebuffers) {
        framebuffer.destroy();
    }
    _current_framebuffer = 1u;

    CONSOLE_WARN("Destroy swapchain");
    _swapchain->destroy();
    CONSOLE_WARN("Reinitialize window surface");
    TargetWindow::init_surface();
    CONSOLE_WARN("Recreate swapchain");
    _swapchain->create();

    // Tie the framebuffers to the swapchain images
    for(uint32_t image_idx = 0;
        image_idx < RenderConfig::swapchain_image_count;
        ++image_idx)
    {
        auto &framebuffer = _framebuffers[image_idx];
        framebuffer.create(_swapchain->extent(),
                            _swapchain->image_view(image_idx),
                            _pipeline->renderpass());
    }

    _camera_data.proj_matrix = glm::perspective(
        RenderConfig::fov_rad * 0.5f,
        RenderConfig::aspect_ratio,
        0.1f,
        1000.0f
    );
}

// =============================================================================
Application::Application() :
    _running             { false },
    _swapchain           { nullptr },
    _pipeline            { nullptr },
    _current_framebuffer { std::numeric_limits<uint32_t>::max() }
{
    ConsoleLog::init();
}

Application::~Application() {
    delete _pipeline;
    delete _swapchain;
    TargetWindow::shutdown();
    GraphicsInstance::shutdown();
}

} // namespace vkl