#ifndef VKLEARNIN_MESHES_PRIMATIVES_PLANE_HPP
#define VKLEARNIN_MESHES_PRIMATIVES_PLANE_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/meshes/Vertex.hpp"
#include "vklearnin/meshes/Index.hpp"
#include "vklearnin/vulkan/resources/vkBuffer.hpp"

namespace vkl {

class vkPhysicalDevice;
class vkDevice;
class vkCmdBuffer;

class Plane final {
public:
    Plane() = default;
    ~Plane() = default;

    Plane(Plane &&) = delete;
    Plane(Plane const &) = delete;

    Plane & operator=(Plane &&) = delete;
    Plane & operator=(Plane const &) = delete;

    bool create(vkPhysicalDevice const &physical_device,
                vkDevice const &device);
    bool destroy();

    bool bind(vkCmdBuffer const &cmd_buffer);
    bool draw(vkCmdBuffer const &cmd_buffer);

    [[nodiscard]] inline auto const & vertex_buffer() const { return _vertex_buffer; }
    [[nodiscard]] inline auto const & index_buffer()  const { return _index_buffer; }

private:
    vkBuffer _vertex_buffer { };
    vkBuffer _index_buffer { };

    static std::vector<Vertex> const _vertices;
    static std::vector<Index::Type> const _indices;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_PRIMATIVES_PLANE_HPP