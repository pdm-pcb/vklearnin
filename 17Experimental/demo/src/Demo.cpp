#include "Demo.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"

size_t CUBES_PER_SIDE = 5;
float  CUBE_STEP      = 0.5f;

// =============================================================================
void Demo::update() {
    auto const cos_yaw   = std::cosf(vkl::math::radians(_cam_data.yaw));
    auto const sin_yaw   = std::sinf(vkl::math::radians(_cam_data.yaw));
    auto const cos_pitch = std::cosf(vkl::math::radians(_cam_data.pitch));
    auto const sin_pitch = std::sinf(vkl::math::radians(_cam_data.pitch));

    _cam_data.forward.x = cos_yaw * cos_pitch;
    _cam_data.forward.y = sin_pitch;
    _cam_data.forward.z = sin_yaw * cos_pitch;

    _cam_data.forward = vkl::math::normalize(_cam_data.forward);
    _cam_data.side = vkl::math::normalize(
        vkl::math::cross(_cam_data.forward, vkl::Vec4::unit_y)
    );
    _cam_data.up = vkl::math::cross(_cam_data.side, _cam_data.forward);

    auto const kb_speed = _cam_data.kb_speed * vkl::Timekeeper::frametime();
    if(_kb.w)          { _cam_data.pos += _cam_data.forward * kb_speed; }
    else if(_kb.s)     { _cam_data.pos -= _cam_data.forward * kb_speed; }
    if(_kb.a)          { _cam_data.pos -= _cam_data.side    * kb_speed; }
    else if(_kb.d)     { _cam_data.pos += _cam_data.side    * kb_speed; }
    if(_kb.space)      { _cam_data.pos += _cam_data.up      * kb_speed; }
    else if(_kb.lctrl) { _cam_data.pos -= _cam_data.up      * kb_speed; }

    _persp_camera.orient(
        _cam_data.pos,
        _cam_data.forward,
        _cam_data.side,
        _cam_data.up
    );

    _vp_matrices.view = _persp_camera.view_matrix();
    _vp_matrices.proj = _persp_camera.proj_matrix();

    vkl::BufferTools::update_buffer(
        _vp_ubos[vkl::Swapchain::image_index()],
        &_vp_matrices
    );
}

// =============================================================================
/*
void Demo::submit_draws() {
    _color_model_matrices.clear();
    _color_model_matrices.push_back(vkl::Mat4::identity);

    vkl::Renderer::submit(
        vkl::Renderer::PipelineType::COLOR,
        {
            .vertex_buffer = _color_cube.vertex_buffer().native(),
            .index_buffer  = _color_cube.index_buffer().native(),
            .index_count   = _color_cube.index_count(),
            .push_constants = {{
                .stage_flags = vk::ShaderStageFlagBits::eVertex,
                .size        = sizeof(vkl::Mat4),
                .data        = &_color_model_matrices.back(),
            }}
        }
    );
}
//*/

//*
void Demo::submit_draws() {
    _color_model_matrices.clear();
    _texture_model_matrices.clear();

    float xpos = std::floorf(CUBES_PER_SIDE * 0.5f) * -CUBE_STEP;
    for(size_t x = 0; x < CUBES_PER_SIDE; ++x) {
        float zpos = std::floorf(CUBES_PER_SIDE * 0.5f) * -CUBE_STEP;

        for(size_t z = 0; z < CUBES_PER_SIDE; ++z) {
            auto translate_top = vkl::math::translate(
                vkl::Mat4::identity,
                { xpos, CUBE_STEP, zpos, 0.0f }
            );

            auto rotate_top = vkl::math::rotate(
                vkl::Mat4::identity,
                vkl::Timekeeper::runtime() * 20.0f,
                { 15.0f, 20.0f, 0.0f, 0.0f }
            );

            auto scale_top = vkl::math::scale(
                vkl::Mat4::identity,
                { 1.5f, 1.0f, 1.0f, 0.0f }
            );

            auto translate_middle = vkl::math::translate(
                vkl::Mat4::identity,
                { xpos, 0.0f, zpos, 0.0f }
            );

            auto rotate_middle = vkl::math::rotate(
                vkl::Mat4::identity,
                vkl::Timekeeper::runtime() * 20.0f,
                { 0.0f, 20.0f, 0.0f, 0.0f }
            );

            auto scale_middle = vkl::math::scale(
                vkl::Mat4::identity,
                { 1.0f, 1.5f, 1.0f, 0.0f }
            );

            auto translate_bottom = vkl::math::translate(
                vkl::Mat4::identity,
                { xpos, -CUBE_STEP, zpos, 0.0f }
            );

            auto rotate_bottom = vkl::math::rotate(
                vkl::Mat4::identity,
                vkl::Timekeeper::runtime() * 20.0f,
                { 0.0f, 20.0f, 25.0f, 0.0f }
            );

            auto scale_bottom = vkl::math::scale(
                vkl::Mat4::identity,
                { 1.0f, 1.0f, 1.5f, 0.0f }
            );

            _texture_model_matrices.emplace_back(
                translate_top *
                rotate_top *
                scale_top
            );
            _color_model_matrices.emplace_back(
                translate_middle *
                rotate_middle *
                scale_middle
            );
            _texture_model_matrices.emplace_back(
                translate_bottom *
                rotate_bottom *
                scale_bottom
            );

            zpos += CUBE_STEP;
        }

        xpos += CUBE_STEP;
    }

    for(auto& matrix : _color_model_matrices) {
        vkl::Renderer::submit(
            vkl::Renderer::PipelineType::COLOR,
            {
                .vertex_buffer = _color_cube.vertex_buffer().native(),
                .index_buffer  = _color_cube.index_buffer().native(),
                .index_count   = _color_cube.index_count(),
                .push_constants = {{
                    .stage_flags = vk::ShaderStageFlagBits::eVertex,
                    .size        = sizeof(vkl::Mat4),
                    .data        = &matrix,
                }}
            }
        );
    }

    for(uint32_t mat_idx = 0u;
        mat_idx < _texture_model_matrices.size();
        ++mat_idx)
    {
        auto material = (mat_idx % 2 == 0) ?
                         _bricks_a.image().handle :
                         _bricks_b.image().handle;

        vkl::Renderer::submit(
            vkl::Renderer::PipelineType::TEXTURE,
            {
                .vertex_buffer = _texture_cube.vertex_buffer().native(),
                .index_buffer  = _texture_cube.index_buffer().native(),
                .index_count   = _texture_cube.index_count(),
                .material      = material,
                .push_constants = {{
                    .stage_flags = vk::ShaderStageFlagBits::eVertex,
                    .size        = sizeof(vkl::Mat4),
                    .data        = &_texture_model_matrices[mat_idx],
                }}
            }
        );
    }
}
//*/

// =============================================================================
void Demo::init() {
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

    vkl::EventBroker::subscribe<vkl::MouseButtonPressEvent>(
        this,
        &Demo::on_mouse_button_press
    );

    vkl::EventBroker::subscribe<vkl::MouseButtonReleaseEvent>(
        this,
        &Demo::on_mouse_button_release
    );

    vkl::EventBroker::subscribe<vkl::MouseScrollEvent>(
        this,
        &Demo::on_mouse_scroll
    );

    _persp_camera.set_perspective(0.1f, 1000.0f, 45.0f);
    // _persp_camera.set_orthographic(1.0f, -1.0f);

#ifdef VKL_USE_GLM
    _cam_data.pos     =  { 0.0f, 0.0f, 3.0f, 0.0f };
    _cam_data.forward =  { 0.0f, 0.0f, -1.0f, 0.0f };
#else
    _cam_data.pos     =  3.0f * vkl::Vec4::unit_z;
    _cam_data.forward = -1.0f * vkl::Vec4::unit_z;
#endif // VKL_USE_GLM

    _vp_ubos.resize(vkl::RenderConfig::image_count);
    for(auto &ubo : _vp_ubos) {
        ubo.size = sizeof(VPMatrices);
        vkl::BufferTools::create(
            ubo,
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        );
    }

    vkl::Renderer::set_global_uniforms(_vp_ubos);

    _color_cube.init(
        0.1f,
        {{
            { 1.0f, 0.0f, 0.0f, 1.0f }, // Red
            { 0.0f, 1.0f, 0.0f, 1.0f }, // Green
            { 0.0f, 0.0f, 1.0f, 1.0f }, // Blue
            { 1.0f, 1.0f, 1.0f, 1.0f }, // White
            { 1.0f, 1.0f, 0.0f, 1.0f }, // Yellow
            { 0.0f, 1.0f, 1.0f, 1.0f }, // Cyan
            { 1.0f, 0.0f, 1.0f, 1.0f }, // Fuchsia
            { 0.0f, 0.0f, 0.0f, 1.0f }, // Black
        }}
    );
    _color_model_matrices.resize(CUBES_PER_SIDE * CUBES_PER_SIDE * 10);

    _texture_cube.init(0.1f, 1.0f);
    _texture_model_matrices.resize(CUBES_PER_SIDE * CUBES_PER_SIDE * 2 * 10);

    _bricks_a.init_from_file("textures/brickwall017_d.jpg");
    _bricks_a.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    vkl::Renderer::add_material(_bricks_a.image());

    _bricks_b.init_from_file("textures/bricks082c_d.jpg");
    _bricks_b.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    vkl::Renderer::add_material(_bricks_b.image());
}

// =============================================================================
void Demo::on_key_press(const vkl::KeyPressEvent &event) {
    switch(event.code) {
        case vkl::KB_W :
            _kb.w = true;
            _kb.s = false;
            break;
        case vkl::KB_A :
            _kb.a = true;
            _kb.d = false;
            break;
        case vkl::KB_S :
            _kb.s = true;
            _kb.w = false;
            break;
        case vkl::KB_D :
            _kb.d = true;
            _kb.a = false;
            break;
        case vkl::KB_LCTRL:
            _kb.lctrl = true;
            _kb.space = false;
            break;
        case vkl::KB_SPACE:
            _kb.space = true;
            _kb.lctrl = false;
            break;

        default: break;
    }
}

// =============================================================================
void Demo::on_key_release(const vkl::KeyReleaseEvent &event) {
    switch(event.code) {
        case vkl::KB_W     : _kb.w     = false; break;
        case vkl::KB_A     : _kb.a     = false; break;
        case vkl::KB_S     : _kb.s     = false; break;
        case vkl::KB_D     : _kb.d     = false; break;
        case vkl::KB_LCTRL : _kb.lctrl = false;  CONSOLE_INFO("LCTRL Up");break;
        case vkl::KB_SPACE : _kb.space = false; break;

        default: break;
    }
}

// =============================================================================
void Demo::on_mouse_move(const vkl::MouseMoveEvent &event) {
    _cam_data.pitch += -event.y_offset * _cam_data.mouse_speed;
    _cam_data.yaw   += event.x_offset * _cam_data.mouse_speed;

    if(_cam_data.pitch > 89.9f)       { _cam_data.pitch = 89.9f;  }
    else if(_cam_data.pitch < -89.9f) { _cam_data.pitch = -89.9f; }
}

// =============================================================================
void Demo::on_mouse_button_press(const vkl::MouseButtonPressEvent &event) {
    switch(event.code) {
        case vkl::MOUSE_BUTTON_LEFT   : CONSOLE_INFO("LMB down"); break;
        case vkl::MOUSE_BUTTON_RIGHT  : CONSOLE_INFO("RMB down"); break;
        case vkl::MOUSE_BUTTON_MIDDLE : CONSOLE_INFO("MMB down"); break;
    }
}

// =============================================================================
void Demo::on_mouse_button_release(const vkl::MouseButtonReleaseEvent &event) {
    switch(event.code) {
        case vkl::MOUSE_BUTTON_LEFT   : CONSOLE_INFO("LMB up"); break;
        case vkl::MOUSE_BUTTON_RIGHT  : CONSOLE_INFO("RMB up"); break;
        case vkl::MOUSE_BUTTON_MIDDLE : CONSOLE_INFO("MMB up"); break;
    }
}

// =============================================================================
void Demo::on_mouse_scroll(const vkl::MouseScrollEvent &event) {
    if(event.vert_offset > 0 && CUBES_PER_SIDE < 100) {
        CUBES_PER_SIDE += 1;
    }
    else if(event.vert_offset < 0 && CUBES_PER_SIDE > 1) {
        CUBES_PER_SIDE -= 1;
    }

    CONSOLE_INFO(
        "{} offset, {} cubes per side",
        event.vert_offset,
        CUBES_PER_SIDE
    );
}

// =============================================================================
void Demo::shutdown() {
    _color_cube.shutdown();
    _texture_cube.shutdown();
    _bricks_a.shutdown();
    _bricks_b.shutdown();
}

// =============================================================================
Demo::Demo() :
    _kb {
        .w = false,
        .a = false,
        .s = false,
        .d = false,
    },
    _vp_matrices  { },
    _persp_camera { },
    _color_cube   { },
    _texture_cube { },
    _bricks_a     { },
    _bricks_b     { }
{ }
