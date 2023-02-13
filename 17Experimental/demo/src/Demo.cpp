#include "Demo.hpp"

#include "vklearnin/rendering/swapchain/Swapchain.hpp"

static size_t constexpr CUBES_PER_SIDE = 5;
static float  constexpr CUBE_STEP      = 0.5f;

// =============================================================================
void Demo::update() {
    auto const fore  = _cam_data.forward.normalized();
    auto const side  = vkl::math::cross(fore, vkl::Vec4::unit_y).normalized();
    auto const up    = vkl::math::cross(fore, side);
    auto const speed = _cam_data.speed * vkl::Timekeeper::frametime();

    if(_kb.w)      { _cam_data.pos += fore * speed; }
    else if(_kb.s) { _cam_data.pos -= fore * speed; }
    if(_kb.a)      { _cam_data.pos -= side * speed; }
    else if(_kb.d) { _cam_data.pos += side * speed; }

    if(_kb.up) {
        _cam_data.forward =
            vkl::math::axis_angle(_cam_data.forward, side, speed * 10.0f);
    }
    else if(_kb.down) {
        _cam_data.forward =
            vkl::math::axis_angle(_cam_data.forward, side, -speed * 10.0f);
    }
    if(_kb.left) {
        _cam_data.forward =
            vkl::math::axis_angle(_cam_data.forward, up, -speed * 10.0f);
    }
    else if(_kb.right) {
        _cam_data.forward =
            vkl::math::axis_angle(_cam_data.forward, up, speed * 10.0f);
    }

    _persp_camera.orient(
        _cam_data.pos,
        // _cam_data.forward
        // { 0.0f, 0.0f, -1.0f } // dead on
        { 0.0f, 0.5f, -1.0f } // camera rotated up
        // { 0.0f, -0.5f, -1.0f } // camera rotated down
    );

    _vp_matrices.view = _persp_camera.view_matrix();
    _vp_matrices.proj = _persp_camera.proj_matrix();

    vkl::BufferTools::update_buffer(
        _view_proj_ubos[vkl::Swapchain::image_index()],
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
                { xpos, CUBE_STEP, zpos }
            );

            auto rotate_top = vkl::math::rotate(
                vkl::Mat4::identity,
                {
                    vkl::Timekeeper::runtime() * 15.0f,
                    vkl::Timekeeper::runtime() * 20.0f,
                    0.0f
                }
            );

            auto translate_middle = vkl::math::translate(
                vkl::Mat4::identity,
                { xpos, 0.0f, zpos }
            );

            auto rotate_middle = vkl::math::rotate(
                vkl::Mat4::identity,
                {
                    0.0f,
                    vkl::Timekeeper::runtime() * 20.0f,
                    0.0f
                }
            );

            auto translate_bottom = vkl::math::translate(
                vkl::Mat4::identity,
                { xpos, -CUBE_STEP, zpos }
            );

            auto rotate_bottom = vkl::math::rotate(
                vkl::Mat4::identity,
                {
                    0.0f,
                    vkl::Timekeeper::runtime() * 20.0f,
                    vkl::Timekeeper::runtime() * 25.0f
                }
            );

            _texture_model_matrices.emplace_back(rotate_top * translate_top);
            _color_model_matrices.emplace_back(rotate_middle * translate_middle);
            _texture_model_matrices.emplace_back(rotate_bottom * translate_bottom);

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

    for(uint32_t mat_idx = 0u; mat_idx < _texture_model_matrices.size(); ++mat_idx) {
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

    _persp_camera.set_perspective(0.1f, 1000.0f, 45.0f);
    _cam_data.pos     =  vkl::Vec4::unit_z * 3.0f;
    _cam_data.forward = -vkl::Vec4::unit_z;

    _view_proj_ubos.resize(vkl::RenderConfig::image_count);
    for(auto &ubo : _view_proj_ubos) {
        ubo.size = sizeof(VPMatrices);
        vkl::BufferTools::create(
            ubo,
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        );
    }

    vkl::Renderer::set_global_uniforms(_view_proj_ubos);

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
    _color_model_matrices.resize(CUBES_PER_SIDE * CUBES_PER_SIDE);

    _texture_cube.init(0.1f, 1.0f);
    _texture_model_matrices.resize(CUBES_PER_SIDE * CUBES_PER_SIDE * 2);

    _bricks_a.init_from_file("textures/brickwall017_d.jpg");
    _bricks_a.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    vkl::Renderer::add_material(_bricks_a.image());

    _bricks_b.init_from_file("textures/bricks082c_d.jpg");
    _bricks_b.init_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    );

    vkl::Renderer::add_material(_bricks_b.image());
}

// =============================================================================
void Demo::on_key_press(const vkl::KeyPressEvent &event) {
    switch(event.keycode) {
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

        case vkl::KB_UP :
            _kb.up = true;
            _kb.down = false;
            break;
        case vkl::KB_DOWN :
            _kb.down = true;
            _kb.up = false;
            break;
        case vkl::KB_LEFT :
            _kb.left = true;
            _kb.right = false;
            break;
        case vkl::KB_RIGHT :
            _kb.right = true;
            _kb.left = false;
            break;

        default: break;
    }
}

// =============================================================================
void Demo::on_key_release(const vkl::KeyReleaseEvent &event) {
    switch(event.keycode) {
        case vkl::KB_W : _kb.w = false; break;
        case vkl::KB_A : _kb.a = false; break;
        case vkl::KB_S : _kb.s = false; break;
        case vkl::KB_D : _kb.d = false; break;

        case vkl::KB_UP    : _kb.up    = false; break;
        case vkl::KB_DOWN  : _kb.down  = false; break;
        case vkl::KB_LEFT  : _kb.left  = false; break;
        case vkl::KB_RIGHT : _kb.right = false; break;

        default: break;
    }
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
    _persp_camera           { },
    _cam_data     { },
    _vp_matrices            { },
    _color_cube             { },
    _texture_cube           { },
    _color_model_matrices   { },
    _texture_model_matrices { }
{ }
