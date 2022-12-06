#ifndef DEMO_CAMERADATA_HPP
#define DEMO_CAMERADATA_HPP

#include "vklearnin/system/pch.hpp"

struct CameraData {
    glm::mat4 view_matrix { 1.0f };
    glm::mat4 proj_matrix { 1.0f };
};

struct CameraOrientation {
    glm::vec3 position { 0.0f, 0.0f, 6.0f };
    glm::vec3 forward  { vkl::math::forward_vec3 };
    glm::vec3 up       { vkl::math::up_vec3 };
    glm::vec3 right    { vkl::math::right_vec3 };

    float pitch = 0.0f;
    float yaw   = 0.0f;
};

struct CameraSettings {
    float speed = 5.0f;
    float sensitivity = 0.075f;
};

#endif // DEMO_CAMERADATA_HPP