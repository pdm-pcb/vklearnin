#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/passes/SkyboxPass.hpp"

namespace vkl {

// =============================================================================
void SkyboxPass::set_texutre(Texture2D::CubeFilepaths const &filepaths) {
    _texture.cubemap_from_files(filepaths);
    _texture.create_sampler(
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        VK_FALSE,
        vk::CompareOp::eAlways
    );
    _texture.generate_mipmap(vk::Filter::eLinear);
}

// =============================================================================
SkyboxPass::SkyboxPass()
{ }

} // namespace vkl