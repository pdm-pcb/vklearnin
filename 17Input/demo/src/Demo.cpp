#include "Demo.hpp"

// =============================================================================
std::vector<vkl::Pipeline *>
Demo::create_pipelines(const vkl::Swapchain &swapchain) {
    _swapchain = &swapchain;
    _pipelines.push_back(new vkl::Pipeline(*_swapchain));

    create_descriptor_pool();

    for(auto &set : _per_frame_sets) {
        set.add_ubo(sizeof(CameraData), vk::ShaderStageFlagBits::eVertex);
        set.create(_descriptor_pool);
    }

    _cube_texture.add_texture2D(
        "../../vklearnin/assets/textures/metal_panel.jpg"
    );
    _cube_texture.create(_descriptor_pool);

    _plane_texture.add_texture2D(
        "../../vklearnin/assets/textures/wooden_wall.jpg"
    );
    _plane_texture.create(_descriptor_pool);

    _pipelines[0]->vertex_from_binary(
        "../../vklearnin/assets/shaders/05texture_sampler.vert-debug.spv"
    );
    _pipelines[0]->fragment_from_binary(
        "../../vklearnin/assets/shaders/05texture_sampler.frag-debug.spv"
    );

    _pipelines[0]->set_push_constants({{
            .stageFlags = vk::ShaderStageFlagBits::eVertex,
            .offset = 0u,
            .size = sizeof(InstanceData)
    }});
    _pipelines[0]->set_per_frame_layout(_per_frame_sets[0].layout().native());
    _pipelines[0]->set_per_material_layout(_cube_texture.layout().native());

    _pipelines[0]->create();
    return _pipelines;
}

// =============================================================================
void Demo::create_descriptor_pool() {
    vkl::PoolSizes pool_sizes {
        {
            .type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 10u,
        },
        {
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 10u,
        },
    };

    _descriptor_pool.create(pool_sizes);
};

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

    _update_camera(frame_index);
    command_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        _pipelines[0]->layout(),
        vkl::Pipeline::BindingFreq::PER_FRAME,
        _per_frame_sets[frame_index].native(),
        { }
    );

    // Go time!
    command_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

        // Establish the area we can draw to
        command_buffer.setViewport(0u, _pipelines[0]->viewport());
        command_buffer.setScissor(0u, _pipelines[0]->scissor());

        InstanceData instance_data { };

//------------------------------------------------------------------------------
// Cube
        command_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _pipelines[0]->layout(),
            vkl::Pipeline::BindingFreq::PER_MATERIAL,
            _cube_texture.native(),
            { }
        );

        auto cube_matrix = glm::rotate(
            glm::translate(glm::mat4(1.0f), { -1.0f, 0.0f, -1.0f }),
            vkl::Timekeeper::runtime() * vkl::math::pi_over_four,
            { 0.75f, 1.0f, 0.0f }
        );
        instance_data.model_matrix = cube_matrix;

        command_buffer.pushConstants<InstanceData>(
            _pipelines[0]->layout(),
            vk::ShaderStageFlagBits::eVertex,
            0u,
            instance_data
        );

        vkl::Renderer::draw(
            command_buffer,
            _unit_cube->vertex_buffer(),
            _unit_cube->index_buffer(),
            static_cast<uint32_t>(_unit_cube->indices().size())
        );

//------------------------------------------------------------------------------
// Plane
        command_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _pipelines[0]->layout(),
            vkl::Pipeline::BindingFreq::PER_MATERIAL,
            _plane_texture.native(),
            { }
        );

        auto plane_matrix = glm::rotate(
            // glm::mat4(1.0f),
            glm::translate(glm::mat4(1.0f), { 1.0f, 0.0f, -1.0f }),
            vkl::Timekeeper::runtime() * vkl::math::pi_over_four,
            { 0.0f, 0.0f, 1.0f }
        );
        instance_data.model_matrix = plane_matrix;

        command_buffer.pushConstants<InstanceData>(
            _pipelines[0]->layout(),
            vk::ShaderStageFlagBits::eVertex,
            0u,
            instance_data
        );

        vkl::Renderer::draw(
            command_buffer,
            _xz_unit_plane->vertex_buffer(),
            _xz_unit_plane->index_buffer(),
            static_cast<uint32_t>(_xz_unit_plane->indices().size())
        );

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
void Demo::on_key_press(const vkl::KeyPressEvent &event) {
    switch(event.keycode) {
        case vkl::KB_W: _kb_state.w = true; break;
        case vkl::KB_A: _kb_state.a = true; break;
        case vkl::KB_S: _kb_state.s = true; break;
        case vkl::KB_D: _kb_state.d = true; break;
        case vkl::KB_LCTRL:  _kb_state.l_ctrl  = true; break;
        case vkl::KB_SPACE:  _kb_state.space   = true; break;
        case vkl::KB_LSHIFT: _kb_state.l_shift = true; break;
    }
}

// =============================================================================
void Demo::on_key_release(const vkl::KeyReleaseEvent &event) {
    switch(event.keycode) {
        case vkl::KB_W: _kb_state.w = false; break;
        case vkl::KB_A: _kb_state.a = false; break;
        case vkl::KB_S: _kb_state.s = false; break;
        case vkl::KB_D: _kb_state.d = false; break;
        case vkl::KB_LCTRL:  _kb_state.l_ctrl  = false; break;
        case vkl::KB_SPACE:  _kb_state.space   = false; break;
        case vkl::KB_LSHIFT: _kb_state.l_shift = false; break;
    }
}

// =============================================================================
void Demo::on_mouse_move(const vkl::MouseMoveEvent &event) {
    auto x_offset = event.x_offset * vkl::Timekeeper::frametime();
    auto y_offset = event.y_offset * vkl::Timekeeper::frametime();

    _camera_orientation.pitch -= _camera_settings.sensitivity * y_offset;
    _camera_orientation.yaw   += _camera_settings.sensitivity * x_offset;
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
        _camera_orientation.position,
        _camera_orientation.forward,
        _camera_orientation.up
    );

    _xz_unit_plane = new vkl::XZUnitPlane;
    _xz_unit_plane->create_buffers();

    _unit_cube = new vkl::UnitCube;
    _unit_cube->create_buffers();

    vkl::EventBroker::subscribe<vkl::KeyPressEvent>(
        this,
        &Demo::on_key_press
    );

    vkl::EventBroker::subscribe<vkl::KeyReleaseEvent>(
        this,
        &Demo::on_key_release
    );

    vkl::EventBroker::subscribe<vkl::MouseMoveEvent>(
        this,
        &Demo::on_mouse_move
    );
}

// =============================================================================
void Demo::shutdown() {
    if(_xz_unit_plane) _xz_unit_plane->destroy_buffers();
    if(_unit_cube)     _unit_cube->destroy_buffers();

    _plane_texture.destroy();
    _cube_texture.destroy();

    for(auto &set : _per_frame_sets) {
        set.destroy();
    }

    for(auto &object : _per_object_sets) {
        for(auto &set : object) {
            set.destroy();
        }
    }

    _descriptor_pool.destroy();

    for(auto *pipeline : _pipelines) {
        pipeline->destroy();
    }
}

// =============================================================================
void Demo::_update_camera(const uint32_t frame_index) {
    glm::vec3 direction { 0.0f };
    float speed = _camera_settings.speed * vkl::Timekeeper::frametime();
    
    if(_kb_state.l_shift) { speed *= 2.0f; }

    if(_kb_state.w) { direction += _camera_orientation.forward * speed; }
    if(_kb_state.a) { direction -= _camera_orientation.right   * speed; }
    if(_kb_state.s) { direction -= _camera_orientation.forward * speed; }
    if(_kb_state.d) { direction += _camera_orientation.right   * speed; }

    if(_kb_state.space)  { direction += _camera_orientation.up * speed; }
    if(_kb_state.l_ctrl) { direction -= _camera_orientation.up * speed; }

    _camera_orientation.position += direction;

    auto T = glm::translate(
        vkl::math::ident_mat4,
        _camera_orientation.position
    );
    auto R = glm::mat4_cast(
        glm::quat({
            _camera_orientation.pitch,
            -_camera_orientation.yaw,
            0.0f
        })
    );

    auto worldmat = T * R;
    _camera_orientation.forward = glm::normalize(
        worldmat * glm::vec4(vkl::math::forward_vec3, 0.0f)
    );
    _camera_orientation.up      = glm::normalize(
        worldmat * glm::vec4(vkl::math::up_vec3, 0.0f)
    );
    _camera_orientation.right  = glm::normalize(
        worldmat * glm::vec4(vkl::math::right_vec3, 0.0f)
    );

    _camera_data.view_matrix = glm::lookAt(
        _camera_orientation.position,
        _camera_orientation.position + _camera_orientation.forward,
        _camera_orientation.up
    );

    _per_frame_sets[frame_index].update_ubo(0, &_camera_data);
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