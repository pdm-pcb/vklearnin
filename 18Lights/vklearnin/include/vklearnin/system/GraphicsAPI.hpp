#ifndef VKLEARNIN_SYSTEM_GRAPHICSAPI_HPP
#define VKLEARNIN_SYSTEM_GRAPHICSAPI_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class GraphicsAPI final {
public:
    static void init();
    static void shutdown();

    inline static auto const& native() { return _instance; }

    GraphicsAPI() = delete;
    ~GraphicsAPI() = delete;

    GraphicsAPI(GraphicsAPI &&other) = delete;
    GraphicsAPI(const GraphicsAPI &other) = delete;

    GraphicsAPI& operator=(GraphicsAPI &&other) = delete;
    GraphicsAPI& operator=(const GraphicsAPI &other) = delete;

private:
    static vk::DynamicLoader _loader;
    static vk::Instance      _instance;

    static vk::ApplicationInfo       _app_info;
    static std::vector<char const *> _enabled_layers;
    static std::vector<char const *> _enabled_extensions;

    using ValidationFeatures = std::vector<vk::ValidationFeatureEnableEXT>;
    static ValidationFeatures        _validation_features;

    static vk::ValidationFeaturesEXT _validation_extensions;

    static vk::InstanceCreateInfo _instance_create_info;

    static void _init_dynamic_loader();
    static void _init_app_info();
    static void _init_layers();
    static void _init_extensions();

    using Extensions = std::vector<vk::ExtensionProperties>;
    static bool _check_extensions(Extensions const &supported_extensions);
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_GRAPHICSAPI_HPP