#include "vklearnin/vklearnin.hpp"
#include "vklearnin/tools/MeshTools.hpp"

namespace vkl {
namespace MeshTools {

void build_surface_normals(std::vector<Vertex> &vertices,
                           const std::vector<Face> &faces)
{
    for(const auto &face : faces) { 
        glm::vec3 v0  = vertices[face.b].position - vertices[face.a].position;
        glm::vec2 uv0 = vertices[face.b].texcoord - vertices[face.a].texcoord;
 
        glm::vec3 v1  = vertices[face.c].position - vertices[face.a].position;
        glm::vec2 uv1 = vertices[face.c].texcoord - vertices[face.a].texcoord;
 
        float r = 1.0f / (uv0[0] * uv1[1] - uv1[0] * uv0[1]);
 
        glm::vec3 tangent = (uv1[1] * v0 - uv0[1] * v1) * r;
        tangent = glm::normalize(tangent);
 
        vertices[face.a].tangent += glm::vec4(tangent, 0.0f);
        vertices[face.b].tangent += glm::vec4(tangent, 0.0f);
        vertices[face.c].tangent += glm::vec4(tangent, 0.0f);
 
        glm::vec3 bitangent = (uv0[0] * v1 - uv1[0] * v0) * r;
        bitangent = glm::normalize(bitangent);
 
        vertices[face.a].bitangent += glm::vec4(bitangent, 0.0f);
        vertices[face.b].bitangent += glm::vec4(bitangent, 0.0f);
        vertices[face.c].bitangent += glm::vec4(bitangent, 0.0f);
 
        glm::vec3 normal = glm::cross(v0, v1);
        normal = glm::normalize(normal);
 
        vertices[face.a].normal += glm::vec4(normal, 0.0f);
        vertices[face.b].normal += glm::vec4(normal, 0.0f);
        vertices[face.c].normal += glm::vec4(normal, 0.0f);
    }
 
    for(uint32_t v_idx = 0; v_idx < vertices.size(); v_idx++) {
        vertices[v_idx].normal    = glm::normalize(vertices[v_idx].normal);
        vertices[v_idx].tangent   = glm::normalize(vertices[v_idx].tangent);
        vertices[v_idx].bitangent = glm::normalize(vertices[v_idx].bitangent);
    }
}

} // namespace MeshTools
} // namespace vkl