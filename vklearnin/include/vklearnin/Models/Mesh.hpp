#ifndef VKLEARNIN_MODELS_MESH_HPP
#define VKLEARNIN_MODELS_MESH_HPP

#include "vklearnin/pch.hpp"
#include "vklearnin/Shaders/Vertex.hpp"
#include "vklearnin/Shaders/Index.hpp"
#include "vklearnin/Shaders/Buffers/BufferObject.hpp"

#include "tiny_gltf.h"

#include <glm/glm.hpp>

class Instance;
class CommandQueues;

class Mesh {
public:
    enum class Primitive {
        XZPlane
    };

    inline const std::vector<vk::Buffer> & vertex_buffers() const {
        return _vertex_buffers;
    }
    inline const std::vector<vk::DeviceSize> & vertex_buffer_offsets() const {
        return _vertex_buffer_offsets;
    }
    inline vk::Buffer index_buffer() const {
        return _index_buffer->handle();
    }
    inline size_t index_count() const {
        return _index_buffer->count();
    }

    void populate_buffers(const CommandQueues &command_queues);

    Mesh(const char *mesh_path, const Instance &instance);
    Mesh(const Primitive primitive, const Instance &instance,
         const float scale, const float u_repeat, const float v_repeat);
    ~Mesh();
    Mesh() = delete;

    Mesh(Mesh &&other) = delete;
    Mesh(const Mesh &other) = delete;

    Mesh & operator=(Mesh &&other) = delete;
    Mesh & operator=(const Mesh &other) = delete;

private:
    std::vector<Vertex> _vertices;
    std::vector<Index>  _indices;

    std::vector<vk::Buffer>     _vertex_buffers;
    std::vector<vk::DeviceSize> _vertex_buffer_offsets;

    BufferObject<Vertex> *_vertex_buffer;
    BufferObject<Index>  *_index_buffer;

    void _process_nodes(tinygltf::Model &model, tinygltf::Node &node);
    void _process_mesh(tinygltf::Model &model,  tinygltf::Mesh &mesh);
    void _build_xzplane(const float scale, const float u_repeat,
                        const float v_repeat);
    void _create_buffer_objects(const Instance &instance);
};

#endif // VKLEARNIN_MODELS_MESH_HPP