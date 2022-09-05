#include "vklearnin/common.hpp"
#include "vklearnin/Models/Model.hpp"

#include "vklearnin/Textures/Texture2D.hpp"

// =============================================================================
const glm::mat4 & Model::update_model_matrix(float runtime) {
    auto T = glm::translate(
        glm::mat4(1.0f),
        _position
    );

    auto R = glm::rotate(
        glm::mat4(1.0f),
        runtime * 0.7854f,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    _model_matrix = T * R;
    // _model_matrix = T;

    return _model_matrix;
}

// =============================================================================
void Model::populate_buffers() {
    _mesh->populate_buffers(_command_queues);
}

// =============================================================================
void Model::init_texture_image_view() {
    _texture->init_image_view();
}

// =============================================================================
void Model::init_texture_sampler(
    const vk::Filter min_filter,
    const vk::Filter mag_filter,
    const vk::SamplerMipmapMode mipmap_mode,
    const vk::SamplerAddressMode address_mode_u,
    const vk::SamplerAddressMode address_mode_v,
    const vk::Bool32 enable_anisotropy)
{
    _texture->init_sampler(min_filter, mag_filter, mipmap_mode,
                           address_mode_u, address_mode_v,
                           enable_anisotropy);
}

// =============================================================================
Model::Model(const char *model_path,
             glm::vec3 position,
             const char *texture_path,
             const CommandQueues &command_queues,
             const Instance &instance) :
    _mesh { new Mesh(model_path, instance) },
    _texture { new Texture2D(command_queues, instance) },
    _position { position },
    _model_matrix  { 1.0f },
    _command_queues { command_queues },
    _instance { instance }
{
    CONSOLE_TRACE("");
    _texture->load_file(texture_path);
}

Model::Model(const Mesh::Primitive primitive, glm::vec3 position,
             const char *texture_path, const CommandQueues &command_queues,
             const Instance &instance,
             const float scale, const float u_repeat, const float v_repeat) :
    _mesh {new Mesh(primitive, instance, scale, u_repeat, v_repeat)},
    _texture { new Texture2D(command_queues, instance) },
    _position { position },
    _model_matrix  { 1.0f },
    _command_queues { command_queues },
    _instance { instance }
{
    CONSOLE_TRACE("");
    _texture->load_file(texture_path);
}

Model::~Model() {
    delete _mesh;
    delete _texture;
}