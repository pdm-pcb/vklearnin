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

    Demo& operator=(Demo &&) = delete;
    Demo& operator=(Demo const&) = delete;

private:
    FPSCamera _camera;

    vkl::Cube<vkl::VertexFlatColor>      _lamp_mesh;
    vkl::Cube<vkl::VertexFlatTexture>    _cube_mesh;
    vkl::XYPlane<vkl::VertexFlatTexture> _wall_mesh;
    vkl::XZPlane<vkl::VertexFlatTexture> _floor_mesh;

    vkl::Mat4 _lamp_matrix;
    vkl::Mat4 _cube_matrix;
    vkl::Mat4 _wall_matrix;
    vkl::Mat4 _floor_matrix;

    vkl::Texture2D _cube_texture;
    vkl::Texture2D _wall_texture;
    vkl::Texture2D _floor_texture;

    void _init_camera();
    void _init_meshes();
    void _init_model_matrices();
    void _init_textures();
    void _init_lights();
};

#endif // DEMO_HPP