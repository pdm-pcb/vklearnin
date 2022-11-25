#include "Demo.hpp"

// =============================================================================
std::vector<vkl::Pipeline *>
Demo::create_pipelines(const vkl::Swapchain &swapchain)
{
    _swapchain = &swapchain;

    _pipelines.push_back(new vkl::Pipeline(*_swapchain));
    _pipelines[0]->vertex_from_binary(
        "../../vklearnin/assets/shaders/05texture_sampler.vert-debug.spv"
    );
    _pipelines[0]->fragment_from_binary(
        "../../vklearnin/assets/shaders/05texture_sampler.frag-debug.spv"
    );
    _pipelines[0]->set_push_constants({
        {
            .stageFlags = vk::ShaderStageFlagBits::eVertex |
                          vk::ShaderStageFlagBits::eFragment,
            .offset = 0u,
            .size = sizeof(vkl::CameraUBO)
        }
    });

    _pipelines[0]->add_ubo(
        sizeof(vkl::CameraUBO),
        vk::ShaderStageFlagBits::eVertex
    );
    // _pipelines[0]->add_ubo(
    //     sizeof(vkl::InstanceUBO),
    //     vk::ShaderStageFlagBits::eVertex
    // );
    _pipelines[0]->add_textures2D(
        { 
            // "../../vklearnin/assets/textures/metal_panel.jpg",
            "../../vklearnin/assets/textures/wooden_wall.jpg"
        },
        vk::ShaderStageFlagBits::eFragment
    );
    _pipelines[0]->create();

    return _pipelines;
}

// =============================================================================
const vk::CommandBuffer & Demo::execute_pipelines(const uint32_t frame_index)
{
    _pipelines[0]->reset_command_pool(frame_index);
 
    // No need for special flags for this application
    vk::CommandBufferBeginInfo begin_info { };

    // Let the command buffer know we're ready to record
    const auto &command_buffer = _pipelines[0]->command_buffer(frame_index);
    auto result = command_buffer.begin(begin_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Failed to begin command buffer recording.");
    }

    // Binding the appropriate pipeline and marking it for drawing commands
    command_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        _pipelines[0]->native()
    );

    // Everybody loves the clear color
    vk::ClearValue clear_values[] = {
        { .color = std::array<float, 4> { 0.08f, 0.08f, 0.16f, 1.0f } },
        { .depthStencil = 1.0f }
    };

    const auto &render_pass = _pipelines[0]->render_pass();

    vk::RenderPassBeginInfo pass_info {
        .renderPass      = render_pass.native(),
        .framebuffer     = render_pass.framebuffer(frame_index).native(),
        .renderArea      = _swapchain->render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
        .pClearValues    = clear_values,
    };

    // _pipelines[0]->update_instance_ubo(&instance_data, frame_index);
    _pipelines[0]->update_camera_ubo(&_camera_data, frame_index);

    // Go time!
    command_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

        // Establish the area we can draw to
        command_buffer.setViewport(0u, _pipelines[0]->viewport());
        command_buffer.setScissor(0u, _pipelines[0]->scissor());

        command_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _pipelines[0]->layout(),
            0u,
            _pipelines[0]->descriptor_set(frame_index).native(),
            { }
        );

        vkl::InstanceUBO instance_data { };

//------------------------------------------------------------------------------
// Plane
        auto plane_matrix = glm::rotate(
            glm::mat4(1.0f),
            // glm::translate(glm::mat4(1.0f), { 1.0f, 0.0f, -1.0f }),
            vkl::Timekeeper::runtime() * vkl::math::pi_over_four,
            { 0.0f, 0.0f, 1.0f }
        );

        instance_data.model_matrix = plane_matrix;
        instance_data.material_index = 1u;

        command_buffer.pushConstants<vkl::InstanceUBO>(
            _pipelines[0]->layout(),
            vk::ShaderStageFlagBits::eVertex |
            vk::ShaderStageFlagBits::eFragment,
            0u,
            instance_data
        );

        vkl::Renderer::draw(
            command_buffer,
            _xz_unit_plane->vertex_buffer(),
            _xz_unit_plane->index_buffer(),
            static_cast<uint32_t>(_xz_unit_plane->indices().size())
        );

//------------------------------------------------------------------------------
// Cube
        // auto cube_matrix = glm::rotate(
        //     glm::mat4(1.0f),
        //     // glm::translate(glm::mat4(1.0f), { -1.0f, 0.0f, -1.0f }),
        //     vkl::Timekeeper::runtime() * vkl::math::pi_over_four,
        //     { 0.75f, 1.0f, 0.0f }
        // );

        // instance_data.model_matrix = cube_matrix,
        // instance_data.material_index = 0u,

        // command_buffer.pushConstants<vkl::InstanceUBO>(
        //     _pipelines[0]->layout(),
        //     vk::ShaderStageFlagBits::eVertex |
        //     vk::ShaderStageFlagBits::eFragment,
        //     0u,
        //     instance_data
        // );

        // vkl::Renderer::draw(
        //     command_buffer,
        //     _unit_cube->vertex_buffer(),
        //     _unit_cube->index_buffer(),
        //     static_cast<uint32_t>(_unit_cube->indices().size())
        // );

// Done Drawing
//------------------------------------------------------------------------------

    // With that out of the way, that's this pass handled
    command_buffer.endRenderPass();

    // And the whole of this command buffer, too
    result = command_buffer.end();
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Failed to end command buffer recording.");
    }

    return command_buffer;
}

// =============================================================================
void Demo::swapchain_image_invalid() {
    _camera_data.proj_matrix = glm::perspective(
        vkl::RenderConfig::fov_rad * 0.5f,
        vkl::RenderConfig::aspect_ratio,
        0.1f,
        1000.0f
    );
}

// =============================================================================
void Demo::init () {
    _camera_data.proj_matrix = glm::perspective(
        vkl::RenderConfig::fov_rad * 0.5f,
        vkl::RenderConfig::aspect_ratio,
        0.1f,
        1000.0f
    );

    _camera_data.view_matrix = glm::lookAt(
        { 0.0f, 0.0f, 3.0f },
        vkl::math::forward_vec3,
        vkl::math::up_vec3
    );

    _xz_unit_plane = new vkl::XZUnitPlane;
    _xz_unit_plane->create_buffers();

    _unit_cube = new vkl::UnitCube;
    _unit_cube->create_buffers();
}

// =============================================================================
void Demo::shutdown() {
    if(_xz_unit_plane) _xz_unit_plane->destroy_buffers();
    if(_unit_cube)     _unit_cube->destroy_buffers();

    for(auto *pipeline : _pipelines) {
        pipeline->destroy();
    }
}

// =============================================================================
Demo::Demo() :
    vkl::Application(),
    _swapchain     { nullptr },
    _camera_data   { },
    _xz_unit_plane { nullptr },
    _unit_cube     { nullptr }
{ }

Demo::~Demo() {
    for(auto *pipeline : _pipelines) {
        delete pipeline;
    }

    delete _xz_unit_plane;
    delete _unit_cube;
}