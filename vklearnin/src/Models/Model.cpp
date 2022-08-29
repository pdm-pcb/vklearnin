#include "vklearnin/common.hpp"
#include "vklearnin/Models/Model.hpp"

void Model::_process_nodes(tinygltf::Model &model, tinygltf::Node  &node) {
    CONSOLE_INFO("");

    for(auto &childnode : node.children) {
        _process_nodes(model, model.nodes[static_cast<size_t>(childnode)]);
    }

    if((node.mesh >= 0) &&
       (static_cast<size_t>(node.mesh) < model.meshes.size()))
    {
        _process_mesh(model, model.meshes[static_cast<size_t>(node.mesh)]);
    }
}

void Model::_process_mesh(tinygltf::Model &model, tinygltf::Mesh  &mesh) {
    CONSOLE_INFO("");

    for(auto &primitive : mesh.primitives) {
        tinygltf::Accessor pos_accessor =
            model.accessors[static_cast<size_t>(primitive.attributes["POSITION"])];
        tinygltf::BufferView pos_buffer_view =
            model.bufferViews[static_cast<size_t>(pos_accessor.bufferView)];
            
        tinygltf::Buffer pos_buffer = model.buffers[static_cast<size_t>(pos_buffer_view.buffer)];
        auto *positions = reinterpret_cast<float*>(
            &pos_buffer.data[pos_buffer_view.byteOffset +
                             pos_accessor.byteOffset]
        );

        tinygltf::Accessor tex_accessor =
            model.accessors[static_cast<size_t>(primitive.attributes["TEXCOORD_0"])];
        tinygltf::BufferView tex_buffer_view =
            model.bufferViews[static_cast<size_t>(tex_accessor.bufferView)];
            
        tinygltf::Buffer tex_buffer = model.buffers[static_cast<size_t>(tex_buffer_view.buffer)];
        auto *texcoords = reinterpret_cast<float*>(
            &tex_buffer.data[tex_buffer_view.byteOffset +
                             tex_accessor.byteOffset]
        );

        if(pos_accessor.count != tex_accessor.count) {
            CONSOLE_WARN("Looks like the GLTF file has a confused number of "
                        "vertex attributes on hand.");
        }

        _vertices.reserve(pos_accessor.count);

        for(size_t vindex = 0; vindex < pos_accessor.count; ++vindex) {
            _vertices.emplace_back(
                Vertex(
                    glm::vec3(
                        positions[vindex * 3 + 0],
                        positions[vindex * 3 + 1],
                        positions[vindex * 3 + 2]
                    ),
                    glm::vec3(1.0f, 1.0f, 1.0f),
                    glm::vec2(
                        texcoords[vindex * 2 + 0],
                        texcoords[vindex * 2 + 1]
                    )
                )
            );
        }

        tinygltf::Accessor idx_accessor =
            model.accessors[static_cast<size_t>(primitive.indices)];
        tinygltf::BufferView idx_buffer_view =
            model.bufferViews[static_cast<size_t>(idx_accessor.bufferView)];
            
        tinygltf::Buffer idx_buffer = model.buffers[static_cast<size_t>(idx_buffer_view.buffer)];
        auto *indices = reinterpret_cast<uint16_t *>(
            &idx_buffer.data[idx_buffer_view.byteOffset +
                             idx_accessor.byteOffset]
        );

        _indices.reserve(idx_accessor.count);
        for(size_t index = 0; index < idx_accessor.count; ++index) {
            _indices.push_back(indices[index]);
        }
    }
}

Model::Model(const char *model_path) {
    CONSOLE_INFO("");

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;

    std::string error;
    std::string warning;

    bool res = loader.LoadASCIIFromFile(&model, &error, &warning, model_path);

    if(!warning.empty()) {
        CONSOLE_WARN("TinyGLTF Warning: {}", warning);
    }
    if(!error.empty()) {
        CONSOLE_ERROR("TinyGLTF Error: {}", warning);
    }
    if(res == false) {
        CONSOLE_ERROR("TinyGLTF failed to load {}", model_path);
    }

    for(auto &node : model.nodes) {
        _process_nodes(model, node);    
    }

    CONSOLE_TRACE("Loaded model with {} vertices", _vertices.size());
}

Model::~Model() {
}