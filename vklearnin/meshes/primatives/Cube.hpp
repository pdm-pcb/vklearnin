// #ifndef VKLEARNIN_MESHES_PRIMATIVES_CUBE_HPP
// #define VKLEARNIN_MESHES_PRIMATIVES_CUBE_HPP

// #include "vklearnin/pch.hpp"

// #include "vklearnin/meshes/Vertex.hpp"
// #include "vklearnin/meshes/Index.hpp"
// #include "vklearnin/vulkan/resources/vkBuffer.hpp"

// namespace vkl {

// class vkPhysicalDevice;
// class vkDevice;
// class vkCmdBuffer;

// class Cube final {
// public:
//     Cube() = default;
//     ~Cube() = default;

//     Cube(Cube &&) = delete;
//     Cube(Cube const &) = delete;

//     Cube & operator=(Cube &&) = delete;
//     Cube & operator=(Cube const &) = delete;

//     bool create(vkDevice const &device);
//     bool destroy();

//     bool bind(vkCmdBuffer const &cmd_buffer);
//     bool draw(vkCmdBuffer const &cmd_buffer);

//     [[nodiscard]] inline auto const & vertex_buffer() const { return _vertex_buffer; }
//     [[nodiscard]] inline auto const & index_buffer()  const { return _index_buffer; }

// private:
//     vkBuffer _vertex_buffer { };
//     vkBuffer _index_buffer { };

//     static std::vector<Vertex> const _vertices;
//     static std::vector<Index::Type> const _indices;
// };

// } // namespace vkl

// #endif // VKLEARNIN_MESHES_PRIMATIVES_CUBE_HPP