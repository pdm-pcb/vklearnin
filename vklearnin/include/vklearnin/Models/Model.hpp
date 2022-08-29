#ifndef VKLEARNIN_MODELS_MODEL_HPP
#define VKLEARNIN_MODELS_MODEL_HPP

#include "vklearnin/Shaders/Vertex.hpp"
#include "vklearnin/Shaders/Index.hpp"

#include "tiny_gltf.h"

#include <string>
#include <vector>
#include <filesystem>

class Mesh;
class MeshLoaded;
class Texture2D;

class Model {
public:
    const std::vector<Vertex> vertices() const { return _vertices; }
    const std::vector<Index>  indices()  const { return _indices;  }

    Model(const char *model_path);
    ~Model();
    Model() = delete;

private:
    std::vector<Vertex> _vertices;
    std::vector<Index>  _indices;

    void _process_nodes(tinygltf::Model &model, tinygltf::Node &node);
    void _process_mesh(tinygltf::Model &model,  tinygltf::Mesh &mesh);
};

#endif // VKLEARNIN_MODELS_MODEL_HPP