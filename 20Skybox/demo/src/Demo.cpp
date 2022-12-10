#include "Demo.hpp"

// =============================================================================
std::vector<vkl::Pipeline *>
Demo::create_pipelines(const vkl::Swapchain &swapchain) {
    create_descriptor_pool();
    _swapchain = &swapchain;

    //
    // Pipeline neutral resources
    //
    for(auto &set : _per_frame_sets) {
        set.add_ubo(sizeof(CameraData), vk::ShaderStageFlagBits::eVertex |
                                        vk::ShaderStageFlagBits::eFragment);
        set.create(_descriptor_pool);
    }

    //
    // Initial pipeline creation
    //
    _pipelines.push_back(new vkl::Pipeline(*_swapchain));
    _pipelines.push_back(new vkl::Pipeline(*_swapchain));
    _pipelines.push_back(new vkl::Pipeline(*_swapchain));
    _object_pipeline = _pipelines[0];
    _envmap_pipeline = _pipelines[1];
    _skybox_pipeline = _pipelines[2];

    // -------------------------------------------------------------------------
    // object pipeline
    _object_pipeline->vertex_from_binary(
        vkl::ASSET_PATH + "/shaders/05texture_sampler.vert-debug.spv"
    );
    _object_pipeline->fragment_from_binary(
        vkl::ASSET_PATH + "/shaders/05texture_sampler.frag-debug.spv"
    );

    _cube_texture.add_texture2D(
        { vkl::ASSET_PATH + "/textures/metal_panel.jpg" }
    );
    _cube_texture.create(_descriptor_pool);

    _xy_plane_texture.add_texture2D(
        { vkl::ASSET_PATH + "/textures/stone_wall.jpg" }
    );
    _xy_plane_texture.create(_descriptor_pool);

    _xz_plane_texture.add_texture2D(
        { vkl::ASSET_PATH + "/textures/gravel.jpg" }
    );
    _xz_plane_texture.create(_descriptor_pool);

    _object_pipeline->set_push_constants({{
            .stageFlags = vk::ShaderStageFlagBits::eVertex,
            .offset = 0u,
            .size = sizeof(InstanceData)
    }});
    _object_pipeline->set_per_frame_layout(_per_frame_sets[0].layout().native());
    _object_pipeline->set_per_material_layout(_cube_texture.layout().native());
    _object_pipeline->set_cull_mode(vk::CullModeFlagBits::eBack);
    _object_pipeline->set_front_face(vk::FrontFace::eCounterClockwise);

    CONSOLE_TRACE("Creating object pipeline");
    _object_pipeline->create();

    // -------------------------------------------------------------------------
    // skybox pipeline
    _skybox_pipeline->vertex_from_binary(
        vkl::ASSET_PATH + "/shaders/06cubemap.vert-debug.spv"
    );
    _skybox_pipeline->fragment_from_binary(
        vkl::ASSET_PATH + "/shaders/06cubemap.frag-debug.spv"
    );

    _skybox_texture.add_texture2D({
        vkl::ASSET_PATH + "/textures/belfast_sunset/px.png",
        vkl::ASSET_PATH + "/textures/belfast_sunset/nx.png",
        vkl::ASSET_PATH + "/textures/belfast_sunset/py.png",
        vkl::ASSET_PATH + "/textures/belfast_sunset/ny.png",
        vkl::ASSET_PATH + "/textures/belfast_sunset/pz.png",
        vkl::ASSET_PATH + "/textures/belfast_sunset/nz.png",
    });
    _skybox_texture.create(_descriptor_pool);

    _skybox_pipeline->set_push_constants({{
            .stageFlags = vk::ShaderStageFlagBits::eVertex,
            .offset = 0u,
            .size = sizeof(InstanceData)
    }});
    _skybox_pipeline->set_per_frame_layout(_per_frame_sets[0].layout().native());
    _skybox_pipeline->set_per_material_layout(_skybox_texture.layout().native());
    _skybox_pipeline->set_cull_mode(vk::CullModeFlagBits::eFront);
    _skybox_pipeline->set_front_face(vk::FrontFace::eCounterClockwise);

    CONSOLE_TRACE("Creating skybox pipeline");
    _skybox_pipeline->create();

    // -------------------------------------------------------------------------
    // envmap pipeline
    _envmap_pipeline->vertex_from_binary(
        vkl::ASSET_PATH + "/shaders/07envmap.vert-debug.spv"
    );
    _envmap_pipeline->fragment_from_binary(
        vkl::ASSET_PATH + "/shaders/07envmap.frag-debug.spv"
    );

    _envmap_pipeline->set_push_constants({{
            .stageFlags = vk::ShaderStageFlagBits::eVertex,
            .offset = 0u,
            .size = sizeof(InstanceData)
    }});
    _envmap_pipeline->set_per_frame_layout(_per_frame_sets[0].layout().native());
    _envmap_pipeline->set_per_material_layout(_skybox_texture.layout().native());
    _envmap_pipeline->set_cull_mode(vk::CullModeFlagBits::eBack);
    _envmap_pipeline->set_front_face(vk::FrontFace::eCounterClockwise);

    CONSOLE_TRACE("Creating envmap pipeline");
    _envmap_pipeline->create();

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
const vk::CommandBuffer & Demo::execute_pipelines(const uint32_t frame_index) {
    _object_pipeline->reset_command_pool(frame_index);

    // Everybody loves the clear color
    vk::ClearValue clear_values[] = {
        { .color = std::array<float, 4> { 0.08f, 0.08f, 0.16f, 1.0f } },
        { .depthStencil = 1.0f }
    };
 
    // No need for special flags for this application
    vk::CommandBufferBeginInfo begin_info { };

    // Let the command buffer know we're ready to record
    const auto &command_buffer = _object_pipeline->command_buffer(frame_index);
    auto result = command_buffer.begin(begin_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Failed to begin command buffer recording.");
    }

    InstanceData instance_data { };

    const auto &render_pass = _object_pipeline->render_pass();
    vk::RenderPassBeginInfo pass_info {
        .renderPass      = render_pass.native(),
        .framebuffer     = render_pass.framebuffer(frame_index).native(),
        .renderArea      = _swapchain->render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
        .pClearValues    = clear_values,
    };

    // Go time!
    command_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

        // First, the camera
        _update_camera(frame_index);
        command_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _object_pipeline->layout(),
            vkl::Pipeline::BindingFreq::PER_FRAME,
            _per_frame_sets[frame_index].native(),
            { }
        );

// //
// // Bind the objects pipeline
// //
//         command_buffer.bindPipeline(
//             vk::PipelineBindPoint::eGraphics,
//             _object_pipeline->native()
//         );

//         // Establish the area we can draw to
//         command_buffer.setViewport(0u, _object_pipeline->viewport());
//         command_buffer.setScissor(0u, _object_pipeline->scissor());

// // -----------------------------------------------------------------------------
// // Cube
//         command_buffer.bindDescriptorSets(
//             vk::PipelineBindPoint::eGraphics,
//             _object_pipeline->layout(),
//             vkl::Pipeline::BindingFreq::PER_MATERIAL,
//             _cube_texture.native(),
//             { }
//         );

//         auto cube_matrix = glm::rotate(
//             glm::translate(vkl::math::ident_mat4, { -4.0f, 0.0f, -2.0f }),
//             vkl::Timekeeper::runtime() * vkl::math::pi_over_four,
//             { 0.75f, 1.0f, 0.0f }
//         );
//         instance_data.model_matrix = cube_matrix;

//         command_buffer.pushConstants<InstanceData>(
//             _object_pipeline->layout(),
//             vk::ShaderStageFlagBits::eVertex,
//             0u,
//             instance_data
//         );

//         vkl::Renderer::draw(
//             command_buffer,
//             _cube->vertex_buffer(),
//             _cube->index_buffer(),
//             static_cast<uint32_t>(_cube->faces().size() * 3)
//         );

// // -----------------------------------------------------------------------------
// // XY Plane
//         command_buffer.bindDescriptorSets(
//             vk::PipelineBindPoint::eGraphics,
//             _object_pipeline->layout(),
//             vkl::Pipeline::BindingFreq::PER_MATERIAL,
//             _xy_plane_texture.native(),
//             { }
//         );

//         auto xy_plane_matrix = glm::rotate(
//             glm::translate(vkl::math::ident_mat4, { 4.0f, 0.0f, -2.0f }),
//             vkl::Timekeeper::runtime() * vkl::math::pi_over_four,
//             { 0.0f, 0.0f, 1.0f }
//         );
//         instance_data.model_matrix = xy_plane_matrix;

//         command_buffer.pushConstants<InstanceData>(
//             _object_pipeline->layout(),
//             vk::ShaderStageFlagBits::eVertex,
//             0u,
//             instance_data
//         );

//         vkl::Renderer::draw(
//             command_buffer,
//             _xy_plane->vertex_buffer(),
//             _xy_plane->index_buffer(),
//             static_cast<uint32_t>(_xy_plane->indices().size())
//         );

// // -----------------------------------------------------------------------------
// // XZ Plane
//         command_buffer.bindDescriptorSets(
//             vk::PipelineBindPoint::eGraphics,
//             _object_pipeline->layout(),
//             vkl::Pipeline::BindingFreq::PER_MATERIAL,
//             _xz_plane_texture.native(),
//             { }
//         );

//         auto xz_plane_matrix = glm::translate(
//             vkl::math::ident_mat4,
//             { 0.0f, -10.0f, 0.0f }
//         );
//         instance_data.model_matrix = xz_plane_matrix;

//         command_buffer.pushConstants<InstanceData>(
//             _object_pipeline->layout(),
//             vk::ShaderStageFlagBits::eVertex,
//             0u,
//             instance_data
//         );

//         vkl::Renderer::draw(
//             command_buffer,
//             _xz_plane->vertex_buffer(),
//             _xz_plane->index_buffer(),
//             static_cast<uint32_t>(_xz_plane->indices().size())
//         );

//
// Bind the envmap pipeline
//
        command_buffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            _envmap_pipeline->native()
        );

        // Establish the area we can draw to
        command_buffer.setViewport(0u, _envmap_pipeline->viewport());
        command_buffer.setScissor(0u, _envmap_pipeline->scissor());

// -----------------------------------------------------------------------------
// Icosphere
        command_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _envmap_pipeline->layout(),
            vkl::Pipeline::BindingFreq::PER_MATERIAL,
            _skybox_texture.native(),
            { }
        );

        auto icosphere_matrix = glm::rotate(
            glm::translate(vkl::math::ident_mat4, { 0.0f, 0.0f, -1.0f }),
            vkl::Timekeeper::runtime() * vkl::math::pi_over_eight,
            { 0.0f, 1.0f, 0.0f }
        );

        instance_data.model_matrix = icosphere_matrix;

        command_buffer.pushConstants<InstanceData>(
            _envmap_pipeline->layout(),
            vk::ShaderStageFlagBits::eVertex,
            0u,
            instance_data
        );

        vkl::Renderer::draw(
            command_buffer,
            _icosphere->vertex_buffer(),
            _icosphere->index_buffer(),
            static_cast<uint32_t>(_icosphere->faces().size() * 3)
        );

//
// Bind the skybox pipeline
//
        command_buffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            _skybox_pipeline->native()
        );

        // Establish the area we can draw to
        command_buffer.setViewport(0u, _skybox_pipeline->viewport());
        command_buffer.setScissor(0u, _skybox_pipeline->scissor());

// -----------------------------------------------------------------------------
// Skybox
        command_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _skybox_pipeline->layout(),
            vkl::Pipeline::BindingFreq::PER_MATERIAL,
            _skybox_texture.native(),
            { }
        );

        instance_data.model_matrix = vkl::math::ident_mat4;
        command_buffer.pushConstants<InstanceData>(
            _skybox_pipeline->layout(),
            vk::ShaderStageFlagBits::eVertex,
            0u,
            instance_data
        );

        vkl::Renderer::draw(
            command_buffer,
            _skybox->vertex_buffer(),
            _skybox->index_buffer(),
            static_cast<uint32_t>(_skybox->faces().size() * 3)
        );

// Done Drawing
// -----------------------------------------------------------------------------

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
        case vkl::KB_W:      _kb_state.w       = true; break;
        case vkl::KB_A:      _kb_state.a       = true; break;
        case vkl::KB_S:      _kb_state.s       = true; break;
        case vkl::KB_D:      _kb_state.d       = true; break;
        case vkl::KB_LCTRL:  _kb_state.l_ctrl  = true; break;
        case vkl::KB_SPACE:  _kb_state.space   = true; break;
        case vkl::KB_LSHIFT: _kb_state.l_shift = true; break;
    }
}

// =============================================================================
void Demo::on_key_release(const vkl::KeyReleaseEvent &event) {
    switch(event.keycode) {
        case vkl::KB_W:      _kb_state.w       = false; break;
        case vkl::KB_A:      _kb_state.a       = false; break;
        case vkl::KB_S:      _kb_state.s       = false; break;
        case vkl::KB_D:      _kb_state.d       = false; break;
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
void Demo::update_projection() {
    _camera_data.proj_matrix = glm::perspective(
        vkl::RenderConfig::fov_rad * 0.5f,
        vkl::RenderConfig::aspect_ratio,
        0.1f,
        5000.0f
    );
}

// =============================================================================
void Demo::init() {
    update_projection();

    _camera_data.view_matrix = glm::lookAt(
        _camera_orientation.position,
        _camera_orientation.forward,
        _camera_orientation.up
    );

    _xy_plane = new vkl::XYPlane(1.0f, 2.0f);
    _xy_plane->create_buffers();

    _xz_plane = new vkl::XZPlane(200.0f, 150.0f);
    _xz_plane->create_buffers();

    _cube = new vkl::Cube(0.5f, 1.0f);
    _cube->create_buffers();

    _skybox = new vkl::Cube(1000.0f, 1.0f);
    _skybox->create_buffers();

    _icosphere = new vkl::Icosphere(2.0f, 5u);
    _icosphere->create_buffers();

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
    if(_xy_plane)  _xy_plane->destroy_buffers();
    if(_xz_plane)  _xz_plane->destroy_buffers();
    if(_cube)      _cube->destroy_buffers();
    if(_skybox)    _skybox->destroy_buffers();
    if(_icosphere) _icosphere->destroy_buffers();

    _xy_plane_texture.destroy();
    _xz_plane_texture.destroy();
    _cube_texture.destroy();
    _skybox_texture.destroy();
    _icosphere_texture.destroy();

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
    _camera_orientation.up = glm::normalize(
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

    _camera_data.position = glm::vec4(_camera_orientation.position, 1.0f);

    _per_frame_sets[frame_index].update_ubo(0, &_camera_data);
}

// =============================================================================
Demo::Demo() :
    vkl::Application(),
    _swapchain          { nullptr },
    _object_pipeline    { nullptr },
    _envmap_pipeline    { nullptr },
    _skybox_pipeline    { nullptr },
    _xy_plane           { nullptr },
    _xz_plane           { nullptr },
    _cube               { nullptr },
    _skybox             { nullptr },
    _icosphere          { nullptr },
    _camera_data        { },
    _camera_orientation { },
    _camera_settings    { },
    _kb_state           { }
{ }

Demo::~Demo() {
    for(auto *pipeline : _pipelines) {
        delete pipeline;
    }

    delete _xy_plane;
    delete _xz_plane;
    delete _cube;
    delete _skybox;
    delete _icosphere;
}