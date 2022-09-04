#ifndef VKLEARNIN_DESCRIPTORSETS_DESCRIPTORSETS_HPP
#define VKLEARNIN_DESCRIPTORSETS_DESCRIPTORSETS_HPP

#include "vklearnin/DescriptorSets/PerFrameDescriptors.hpp"
#include "vklearnin/DescriptorSets/PerPassDescriptors.hpp"
#include "vklearnin/DescriptorSets/PerMaterialDescriptors.hpp"
#include "vklearnin/DescriptorSets/PerObjectDescriptors.hpp"

struct DescriptorSets {
    PerFrameDescriptors    &per_frame;
    PerPassDescriptors     &per_pass;
    PerMaterialDescriptors &per_material;
    PerObjectDescriptors   &per_object;
};

#endif // VKLEARNIN_DESCRIPTORSETS_DESCRIPTORSETS_HPP