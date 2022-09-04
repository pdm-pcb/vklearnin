#include "vklearnin/common.hpp"
#include "vklearnin/Models/Mesh.hpp"

#include "vklearnin/CommandStructures/CommandQueues.hpp"

// =============================================================================
void
Mesh::populate_buffers(const CommandQueues &command_queues) {
    CONSOLE_INFO("");
    
    _vertex_buffer->populate_buffer(
        command_queues.command_pool(),
        command_queues.graphics_queue()
    );
    _index_buffer->populate_buffer(
        command_queues.command_pool(),
        command_queues.graphics_queue()
    );

    _vertex_buffers.emplace_back(_vertex_buffer->handle());
    _vertex_buffer_offsets.emplace_back(0u);
}

// =============================================================================
void Mesh::_process_nodes(tinygltf::Model &model, tinygltf::Node  &node) {
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

// =============================================================================
void Mesh::_process_mesh(tinygltf::Model &model, tinygltf::Mesh  &mesh) {
    CONSOLE_INFO("");

    for(auto &primitive : mesh.primitives) {
        tinygltf::Accessor pos_accessor = model.accessors[
            static_cast<size_t>(primitive.attributes["POSITION"])
        ];
        tinygltf::BufferView pos_buffer_view =
            model.bufferViews[static_cast<size_t>(pos_accessor.bufferView)];
            
        tinygltf::Buffer pos_buffer =
            model.buffers[static_cast<size_t>(pos_buffer_view.buffer)];
        auto *positions = reinterpret_cast<float*>(
            &pos_buffer.data[pos_buffer_view.byteOffset +
                             pos_accessor.byteOffset]
        );

        tinygltf::Accessor tex_accessor = model.accessors[
            static_cast<size_t>(primitive.attributes["TEXCOORD_0"])
        ];
        tinygltf::BufferView tex_buffer_view =
            model.bufferViews[static_cast<size_t>(tex_accessor.bufferView)];
            
        tinygltf::Buffer tex_buffer =
            model.buffers[static_cast<size_t>(tex_buffer_view.buffer)];
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

// =============================================================================
void Mesh::_build_xzplane(const float scale, const float u_repeat,
                           const float v_repeat)
{
    _vertices = {
        {{ -0.5f * scale, 0.0f, 0.5f * scale}, { 1.0f, 1.0f, 1.0f },
         { 0.0f * u_repeat, 0.0f * v_repeat }},

        {{  0.5f * scale, 0.0f, 0.5f * scale}, { 1.0f, 1.0f, 1.0f },
         { 1.0f * u_repeat, 0.0f * v_repeat }},

        {{  0.5f * scale, 0.0f, -0.5f * scale}, { 1.0f, 1.0f, 1.0f },
         { 1.0f * u_repeat, 1.0f * v_repeat }},

        {{ -0.5f * scale, 0.0f, -0.5f * scale}, { 1.0f, 1.0f, 1.0f },
         { 0.0f * u_repeat, 1.0f * v_repeat }}
    };

    _indices = {
        0, 1, 2,
        2, 3, 0
    };
}

// =============================================================================
void Mesh::_create_buffer_objects(const Instance &instance) {
    _vertex_buffer = new BufferObject<Vertex>(_vertices, instance, "vbo");
    _index_buffer  = new BufferObject<Index>(_indices, instance, "ibo");
}

// =============================================================================
Mesh::Mesh(const char *mesh_path, glm::vec3 position,
             const Instance &instance) :
    _vertex_buffer { nullptr  },
    _index_buffer  { nullptr  }
{
    CONSOLE_INFO("");

    tinygltf::TinyGLTF loader;
    tinygltf::Model model;

    std::string error;
    std::string warning;

    bool res = loader.LoadASCIIFromFile(&model, &error, &warning, mesh_path);
    // bool res = loader.LoadBinaryFromFile(&model, &error, &warning, model_path);

    if(!warning.empty()) {
        CONSOLE_WARN("TinyGLTF Warning: {}", warning);
    }
    if(!error.empty()) {
        CONSOLE_ERROR("TinyGLTF Error: {}", error);
    }
    if(res == false) {
        CONSOLE_CRITICAL("TinyGLTF failed to load {}", mesh_path);
    }

    for(auto &node : model.nodes) {
        _process_nodes(model, node);    
    }

    CONSOLE_TRACE("Loaded model with {} vertices", _vertices.size());
    _create_buffer_objects(instance);
}

Mesh::Mesh(const Primitive primitive, glm::vec3 position,
           const Instance &instance,
           const float scale, const float u_repeat, const float v_repeat) :
    _vertex_buffer { nullptr  },
    _index_buffer  { nullptr  }
{
    CONSOLE_INFO("");

    switch(primitive) {
        case Primitive::XZPlane:
            _vertices.reserve(4);
            _indices.reserve(6);
            _build_xzplane(scale, u_repeat, v_repeat);
            break;
    }

    CONSOLE_TRACE("Generated model with {} vertices", _vertices.size());
    _create_buffer_objects(instance);
}

Mesh::~Mesh() {
    CONSOLE_INFO("");
    
    delete _vertex_buffer;
    delete _index_buffer;
}