#ifndef VKLEARNIN_MODELS_MODEL_HPP
#define VKLEARNIN_MODELS_MODEL_HPP

#include "vklearnin/pch.hpp"
#include "vklearnin/Models/Mesh.hpp"

class Texture2D;
class CommandQueues;
class Instance;

class Model {
public:
    const glm::mat4 & update_model_matrix(float runtime);

    Mesh      const * mesh()    const { return _mesh;    }
    Texture2D const * texture() const { return _texture; }

    void populate_buffers();
    void init_texture_image_view();
    void init_texture_sampler(
        const vk::Filter min_filter = vk::Filter::eLinear,
        const vk::Filter mag_filter = vk::Filter::eLinear,
        const vk::SamplerMipmapMode mipmap_mode =
            vk::SamplerMipmapMode::eLinear,
        const vk::SamplerAddressMode address_mode_u =
            vk::SamplerAddressMode::eRepeat,
        const vk::SamplerAddressMode address_mode_v =
            vk::SamplerAddressMode::eRepeat,
        const vk::Bool32 enable_anisotropy = true
    );

    Model(const char *model_path, glm::vec3 position,
          const char *texture_path, const CommandQueues &command_queues,
          const Instance &instance);
    Model(const Mesh::Primitive primitive, glm::vec3 position,
          const char *texture_path, const CommandQueues &command_queues,
          const Instance &instance,
          const float scale = 1.0f,
          const float u_repeat = 1.0f,
          const float v_repeat = 1.0f);
    ~Model();
    Model() = delete;

    Model(Model &&other) = delete;
    Model(const Model &other) = delete;

    Model & operator=(Model &&other) = delete;
    Model & operator=(const Model &other) = delete;

private:
    Mesh      *_mesh;
    Texture2D *_texture;

    glm::vec3 _position;
    glm::mat4 _model_matrix;

    const CommandQueues &_command_queues;
    const Instance &_instance;
};

#endif // VKLEARNIN_MODELS_MODEL_HPP