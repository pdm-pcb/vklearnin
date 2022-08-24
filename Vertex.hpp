#ifndef VKL_VERTEX_HPP
#define VKL_VERTEX_HPP

#include "common.hpp"

class Vertex final {
public:
    using AttribDesc = std::array<::VkVertexInputAttributeDescription, 2>;
    using BindingDesc = ::VkVertexInputBindingDescription;

    static inline BindingDesc binding_desc() {
        return BindingDesc {
            .binding = 0u,
            .stride  = static_cast<uint32_t>(sizeof(Vertex)),
            .inputRate = ::VK_VERTEX_INPUT_RATE_VERTEX
        };
    }

    static inline AttribDesc attribute_desc() {
        return AttribDesc {
            ::VkVertexInputAttributeDescription {
                .location = 0u,
                .binding = 0u,
                .format = ::VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(Vertex, _position)
            },
            ::VkVertexInputAttributeDescription {
                .location = 1u,
                .binding = 0u,
                .format = ::VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(Vertex, _color)
            },
        };
    }

    inline void set_position(const glm::vec2 &position) {
        _position = position;
    }

    inline void set_color(const glm::vec3 &color) {
        _color = color;
    }

    Vertex() = default;
    Vertex(const glm::vec2 &position, const glm::vec3 &color) :
        _position { position },
        _color { color }
    { }

private:
    glm::vec2 _position;
    glm::vec3 _color;
};

#endif // VKL_VERTEX_HPP