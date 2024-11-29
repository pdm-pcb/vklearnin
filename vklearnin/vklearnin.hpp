#ifndef VKLEARNIN_VKLEARNIN_HPP
#define VKLEARNIN_VKLEARNIN_HPP

namespace vkl {

#ifdef VKL_DEBUG
    static std::filesystem::path const VKL_ASSET_PATH("assets/");
    static std::string           const VKL_SHADER_EXT("-debug.spv");
#else
    static std::filesystem::path const VKL_ASSET_PATH("assets/");
    static std::string           const VKL_SHADER_EXT(".spv");
#endif // VKL build config

} // namespace vkl

#endif // VKLEARNIN_VKLEARNIN_HPP