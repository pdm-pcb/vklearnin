#ifndef VKLEARNIN_MODELS_MODEL_HPP
#define VKLEARNIN_MODELS_MODEL_HPP

#include "vklearnin/Shaders/Vertex.hpp"
#include "vklearnin/Shaders/Index.hpp"
#include "vklearnin/Shaders/Buffers/BufferObject.hpp"

#include "tiny_gltf.h"

#include <glm/glm.hpp>

class Instance;

class Model {
public:
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

    void populate_buffers(const vk::CommandPool &pool, const vk::Queue &queue);

    glm::mat4 model_matrix;

    Model(const char *model_path, const Instance &instance);
    ~Model();
    Model() = delete;

    Model(Model &&other) = delete;
    Model(const Model &other) = delete;

    Model & operator=(Model &&other) = delete;
    Model & operator=(const Model &other) = delete;

private:
    std::vector<Vertex> _vertices;
    std::vector<Index>  _indices;

    std::vector<vk::Buffer>     _vertex_buffers;
    std::vector<vk::DeviceSize> _vertex_buffer_offsets;

    BufferObject<Vertex> *_vertex_buffer;
    BufferObject<Index>  *_index_buffer;

    void _process_nodes(tinygltf::Model &model, tinygltf::Node &node);
    void _process_mesh(tinygltf::Model &model,  tinygltf::Mesh &mesh);
};

#endif // VKLEARNIN_MODELS_MODEL_HPP