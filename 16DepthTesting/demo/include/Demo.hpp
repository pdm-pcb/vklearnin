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

////////////////////////////////////////////////////////////////////////////////
// TODO: remove this and/or make it client-side exclusive
#include "vklearnin/shaders/CameraUBO.hpp"
#include "vklearnin/shaders/InstanceUBO.hpp"
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// TODO: replace with proper asset management
#include "vklearnin/mesh/XZUnitPlane.hpp"
#include "vklearnin/mesh/UnitCube.hpp"
////////////////////////////////////////////////////////////////////////////////

struct DrawObject {

};

class Demo final : public vkl::Application {
public:
    std::vector<vkl::Pipeline *>
    create_pipelines(const vkl::Swapchain &swapchain) override;
    void create_descriptor_pool() override;

    const vk::CommandBuffer &
    execute_pipelines(const uint32_t frame_index) override;

    void swapchain_image_invalid();

    void init() override;
    void shutdown() override;

    Demo();
    ~Demo();

private:
    vkl::Swapchain const *_swapchain;
    std::vector<vkl::Pipeline *> _pipelines;

    vkl::CameraUBO _camera_data;

    vkl::XZUnitPlane *_xz_unit_plane;
    vkl::UnitCube    *_unit_cube;

    using FrameSets = std::array<vkl::DescriptorSet,
                                 vkl::RenderConfig::swapchain_image_count>;

    FrameSets _per_frame_sets;
    std::vector<FrameSets> _per_object_sets;
    vkl::DescriptorSet _plane_texture;
    vkl::DescriptorSet _cube_texture;
};

#endif // DEMO_HPP