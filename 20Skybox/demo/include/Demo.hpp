//
// The Demo class is a little playground which is used to prove out the
// rendering library that makes up the bulk of this project.
//
// It's required that at least one class in the user-side code inherits from
// vkl::Application.
//

#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"

#include "CameraData.hpp"
#include "InstanceData.hpp"

////////////////////////////////////////////////////////////////////////////////
// TODO: replace with proper asset management
#include "vklearnin/mesh/XYPlane.hpp"
#include "vklearnin/mesh/XZPlane.hpp"
#include "vklearnin/mesh/Cube.hpp"
#include "vklearnin/mesh/Icosphere.hpp"
////////////////////////////////////////////////////////////////////////////////

class Demo final : public vkl::Application {
public:
    std::vector<vkl::Pipeline *>
    create_pipelines(const vkl::Swapchain &swapchain) override;
    void create_descriptor_pool() override;

    const vk::CommandBuffer &
    execute_pipelines(const uint32_t frame_index) override;

    void on_key_press(const vkl::KeyPressEvent &event);
    void on_key_release(const vkl::KeyReleaseEvent &event);
    void on_mouse_move(const vkl::MouseMoveEvent &event);

    void update_projection();

    void init() override;
    void shutdown() override;

    Demo();
    ~Demo();

private:
    struct KeyboardState {
        bool w       = false;
        bool a       = false;
        bool s       = false;
        bool d       = false;
        bool l_ctrl  = false;
        bool space   = false;
        bool l_shift = false;
    };

    vkl::Swapchain const *_swapchain;
    std::vector<vkl::Pipeline *> _pipelines;

    using FrameSets = std::array<vkl::DescriptorSet,
                                 vkl::RenderConfig::swapchain_image_count>;

    FrameSets              _per_frame_sets;
    std::vector<FrameSets> _per_object_sets;

    vkl::XYPlane   *_xy_plane;
    vkl::XZPlane   *_xz_plane;
    vkl::Cube      *_cube;
    vkl::Cube      *_skybox;
    vkl::Icosphere *_icosphere;

    vkl::DescriptorSet _xy_plane_texture;
    vkl::DescriptorSet _xz_plane_texture;
    vkl::DescriptorSet _cube_texture;
    vkl::DescriptorSet _skybox_texture;

    CameraData        _camera_data;
    CameraOrientation _camera_orientation;
    CameraSettings    _camera_settings;
    KeyboardState     _kb_state;

    void _update_camera(const uint32_t frame_index);
};

#endif // DEMO_HPP