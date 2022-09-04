#ifndef VKLEARNIN_SHADERS_BUFFERS_UBOLIST_HPP
#define VKLEARNIN_SHADERS_BUFFERS_UBOLIST_HPP

#include "vklearnin/Shaders/Buffers/UniformBufferObject.hpp"

struct UBOList {
    UniformBufferObject &per_frame;
    UniformBufferObject &per_object;
};

#endif // VKLEARNIN_SHADERS_BUFFERS_UBOLIST_HPP