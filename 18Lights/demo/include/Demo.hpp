#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Cube.hpp"
#include "vklearnin/meshes/Plane.hpp"

#include "FPSCamera.hpp"

class Demo final : public vkl::Application {
public:
    void update()       override;
    void submit_draws() override;
    void init()         override;
    void shutdown()     override;

    Demo();
    ~Demo() override = default;

    Demo(Demo &&) = delete;
    Demo(Demo const&) = delete;

    Demo & operator=(Demo &&) = delete;
    Demo & operator=(Demo const&) = delete;

private:
    FPSCamera _camera;

    vkl::Cube  _dir_lamp_mesh;
    vkl::Cube  _point_lamp_mesh;
    vkl::Cube  _spot_lamp_mesh;
    vkl::Cube  _cube_mesh;
    vkl::Plane _floor_mesh;
    vkl::Plane _wall_mesh;

    vkl::Mat4 _dir_lamp_matrix;
    vkl::Mat4 _point_lamp_matrix;
    vkl::Mat4 _spot_lamp_matrix;
    vkl::Mat4 _cube_matrix_a;
    vkl::Mat4 _cube_matrix_b;
    vkl::Mat4 _floor_matrix;
    vkl::Mat4 _wall_matrix_a;
    vkl::Mat4 _wall_matrix_b;

    vkl::Material _cube_material;
    vkl::Material _floor_material;
    vkl::Material _wall_material;

    void _init_camera();
    void _init_meshes();
    void _init_model_matrices();
    void _init_textures();
};

#endif // DEMO_HPP