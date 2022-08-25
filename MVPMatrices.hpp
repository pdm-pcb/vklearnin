#ifndef VKL_MVPMATRICES_HPP
#define VKL_MVPMATRICES_HPP

#include <glm/glm.hpp>

struct MVPMatrices {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

#endif // VKL_MVPMATRICES_HPP