#ifndef VKLEARNIN_VERTEX_HPP
#define VKLEARNIN_VERTEX_HPP

#include "vklearnin/pch.hpp"

class Vertex final {
public:
    using AttribDesc = std::vector<vk::VertexInputAttributeDescription>;
    using BindingDesc = std::vector<vk::VertexInputBindingDescription>;

    static inline BindingDesc binding_desc() {
        return BindingDesc {
            {
                .binding = 0u,
                .stride  = static_cast<uint32_t>(sizeof(Vertex)),
                .inputRate = vk::VertexInputRate::eVertex
            }
        };
    }

    static inline AttribDesc attribute_desc() {
        return AttribDesc {
            vk::VertexInputAttributeDescription {
                .location = 0u,
                .binding = 0u,
                .format = vk::Format::eR32G32B32Sfloat,
                .offset = offsetof(Vertex, _position)
            },
            vk::VertexInputAttributeDescription {
                .location = 1u,
                .binding = 0u,
                .format = vk::Format::eR32G32B32Sfloat,
                .offset = offsetof(Vertex, _color)
            },
            vk::VertexInputAttributeDescription {
                .location = 2u,
                .binding = 0u,
                .format = vk::Format::eR32G32Sfloat,
                .offset = offsetof(Vertex, _texcoord)
            },
        };
    }

    Vertex(const glm::vec3 &position, const glm::vec3 &color,
           const glm::vec2 &texcoord) :
        _position { position },
        _color    { color },
        _texcoord { texcoord }
    { }
    
    Vertex() = delete;

private:
    glm::vec3 _position;
    glm::vec3 _color;
    glm::vec2 _texcoord;
};

#endif // VKLEARNIN_VERTEX_HPP