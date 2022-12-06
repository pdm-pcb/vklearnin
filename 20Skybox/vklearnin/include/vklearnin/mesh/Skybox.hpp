#ifndef VKLEARNIN_MESH_SKYBOX_HPP
#define VKLEARNIN_MESH_SKYBOX_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/mesh/Vertex.hpp"
#include "vklearnin/shaders/BufferObject.hpp"

namespace vkl {

class LogicalDevice;

class Skybox final {
public:
    void create_buffers();
    void destroy_buffers();

    inline const auto & vertex_buffer() const { return _vertex_buffer; }
    inline auto & vertices() { return _vertices; }

    inline const auto & index_buffer() const { return _index_buffer; }
    inline auto & indices() { return _indices; }

    explicit Skybox(const float scale);
    ~Skybox() = default;
    Skybox() = delete;

    Skybox(Skybox &&) = delete;
    Skybox(const Skybox &) = delete;

    Skybox & operator=(Skybox &&) = delete;
    Skybox & operator=(const Skybox &) = delete;

private:
    std::vector<Vertex>   _vertices;
    std::vector<uint32_t> _indices;

    BufferObject _vertex_buffer;
    BufferObject _index_buffer;
};

} // namespace vkl

#endif // VKLEARNIN_MESH_SKYBOX_HPP