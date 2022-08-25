#ifndef VKLEARNIN_MVPMATRICES_HPP
#define VKLEARNIN_MVPMATRICES_HPP

#include <glm/glm.hpp>

struct MVPMatrices {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

#endif // VKLEARNIN_MVPMATRICES_HPP