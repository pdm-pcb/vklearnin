#ifndef DEMO_HPP
#define DEMO_HPP

#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Cube.hpp"
#include "vklearnin/meshes/XYPlane.hpp"
#include "vklearnin/meshes/XZPlane.hpp"

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

    vkl::Cube<vkl::VertexFlatColor>   _lamp_mesh;
    vkl::Cube<vkl::VertexLitColor>    _cube_mesh;
    vkl::XZPlane<vkl::VertexLitColor> _floor_mesh;
    vkl::XYPlane<vkl::VertexLitColor> _wall_mesh;

    vkl::Mat4 _lamp_matrix;
    vkl::Mat4 _cube_matrix;
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