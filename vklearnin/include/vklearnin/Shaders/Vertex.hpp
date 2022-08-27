#ifndef VKLEARNIN_VERTEX_HPP
#define VKLEARNIN_VERTEX_HPP

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <vector>

class Vertex final {
public:
    using AttribDesc = std::vector<::VkVertexInputAttributeDescription>;
    using BindingDesc = std::vector<::VkVertexInputBindingDescription>;

    static inline BindingDesc binding_desc() {
        return BindingDesc {
            {
                .binding = 0u,
                .stride  = static_cast<uint32_t>(sizeof(Vertex)),
                .inputRate = ::VK_VERTEX_INPUT_RATE_VERTEX
            }
        };
    }

    static inline AttribDesc attribute_desc() {
        return AttribDesc {
            ::VkVertexInputAttributeDescription {
                .location = 0u,
                .binding = 0u,
                .format = ::VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(Vertex, _position)
            },
            ::VkVertexInputAttributeDescription {
                .location = 1u,
                .binding = 0u,
                .format = ::VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(Vertex, _color)
            },
            ::VkVertexInputAttributeDescription {
                .location = 2u,
                .binding = 0u,
                .format = ::VK_FORMAT_R32G32_SFLOAT,
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