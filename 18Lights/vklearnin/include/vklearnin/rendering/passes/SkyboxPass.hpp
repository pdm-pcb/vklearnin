#ifndef VKLEARNIN_RENDERING_PASSES_SKYBOXPASS_HPP
#define VKLEARNIN_RENDERING_PASSES_SKYBOXPASS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/pipeline/Pipeline.hpp"
#include "vklearnin/meshes/Skybox.hpp"
#include "vklearnin/rendering/descriptors/DescriptorSet.hpp"
#include "vklearnin/rendering/descriptors/DescriptorPool.hpp"
#include "vklearnin/resources/images/Texture2D.hpp"

namespace vkl {

class RenderGraph;

class SkyboxPass {
public:
    SkyboxPass();
    ~SkyboxPass() = default;

    SkyboxPass(SkyboxPass &&) = delete;
    SkyboxPass(const SkyboxPass &) = delete;

    SkyboxPass & operator=(SkyboxPass &&) = delete;
    SkyboxPass & operator=(const SkyboxPass &) = delete;

private:
    Pipeline      _pipeline;
    Skybox        _mesh;
    DescriptorSet _texture_set;
    Texture2D     _texture;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_PASSES_SKYBOXPASS_HPP