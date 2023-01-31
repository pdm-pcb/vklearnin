#ifndef VKLEARNIN_MESHES_VERTEX_HPP
#define VKLEARNIN_MESHES_VERTEX_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Vertex final {
public:
    using BindingDescriptions = std::vector<vk::VertexInputBindingDescription>;
    using AttribDescriptions = std::vector<vk::VertexInputAttributeDescription>;

    inline static const auto& binding_desc() { return _binding_desc; }
    inline static const auto& attrib_desc()  { return _attrib_desc;  }

    Vertex(const std::array<float, 4> &position,
           const std::array<float, 4> &color);
    Vertex() = delete;

private:
    static const BindingDescriptions _binding_desc;
    static const AttribDescriptions  _attrib_desc;

    std::array<float, 4> _position;
    std::array<float, 4> _color;
};

} // namespace vkl

#endif // VKLEARNIN_MESHES_VERTEX_HPP