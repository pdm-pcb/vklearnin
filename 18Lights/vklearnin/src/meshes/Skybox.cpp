#include "vklearnin/vklearnin.hpp"
#include "vklearnin/meshes/Skybox.hpp"

namespace vkl {

void Skybox::init(float const scale) {
    GeneratedMesh::_set_vertices({
        // +x
        { .position = {  scale, -scale, -scale, 1.0f }}, // 0
        { .position = {  scale,  scale, -scale, 1.0f }}, // 1
        { .position = {  scale,  scale,  scale, 1.0f }}, // 2
        { .position = {  scale, -scale,  scale, 1.0f }}, // 3

        // -x
        { .position = { -scale, -scale,  scale, 1.0f }}, // 4
        { .position = { -scale,  scale,  scale, 1.0f }}, // 5
        { .position = { -scale,  scale, -scale, 1.0f }}, // 6
        { .position = { -scale, -scale, -scale, 1.0f }}, // 7

        // +y
        { .position = { -scale,  scale, -scale, 1.0f }}, // 8
        { .position = { -scale,  scale,  scale, 1.0f }}, // 9
        { .position = {  scale,  scale,  scale, 1.0f }}, // 10
        { .position = {  scale,  scale, -scale, 1.0f }}, // 11

        // -y
        { .position = { -scale, -scale,  scale, 1.0f }}, // 12
        { .position = { -scale, -scale, -scale, 1.0f }}, // 13
        { .position = {  scale, -scale, -scale, 1.0f }}, // 14
        { .position = {  scale, -scale,  scale, 1.0f }}, // 15

        // +z
        { .position = {  scale, -scale,  scale, 1.0f }}, // 16
        { .position = {  scale,  scale,  scale, 1.0f }}, // 17
        { .position = { -scale,  scale,  scale, 1.0f }}, // 18
        { .position = { -scale, -scale,  scale, 1.0f }}, // 19

        // -z
        { .position = { -scale, -scale, -scale, 1.0f }}, // 20
        { .position = { -scale,  scale, -scale, 1.0f }}, // 21
        { .position = {  scale,  scale, -scale, 1.0f }}, // 22
        { .position = {  scale, -scale, -scale, 1.0f }}, // 23
    });

    GeneratedMesh::_set_indices({
        // +x
        0, 1, 2,
        0, 2, 3,

        // -x
        4, 5, 6,
        4, 6, 7,

        // +y
        8,  9, 10,
        8, 10, 11,

        // -y
        12, 13, 14,
        12, 14, 15,

        // +z
        16, 17, 18,
        16, 18, 19,

        // -z
        20, 21, 22,
        20, 22, 23,
    });
}

} // namespace vkl