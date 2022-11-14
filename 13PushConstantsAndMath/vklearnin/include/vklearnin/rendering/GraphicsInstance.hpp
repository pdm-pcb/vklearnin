#ifndef VKLEARNIN_RENDERING_GRAPHICSINSTANCE_HPP
#define VKLEARNIN_RENDERING_GRAPHICSINSTANCE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class GraphicsInstance final {
public:
    using ValidationFeatures = std::vector<vk::ValidationFeatureEnableEXT>;

    static void init();
    static void init_devices();
    static void shutdown();

    inline static const vk::Instance & native() { return _graphics_instance; }

    GraphicsInstance() = delete;
    ~GraphicsInstance() = delete;

    GraphicsInstance(GraphicsInstance &&other) = delete;
    GraphicsInstance(const GraphicsInstance &other) = delete;

    GraphicsInstance & operator=(GraphicsInstance &&other) = delete;
    GraphicsInstance & operator=(const GraphicsInstance &other) = delete;

private:
    // Thanks to Vulkan-hpp, we can automate the loading of zillions of
    // function pointers
    static vk::DynamicLoader _loader;
    static vk::Instance      _graphics_instance;

    static vk::ApplicationInfo       _app_info;
    static std::vector<const char *> _enabled_layers;
    static std::vector<const char *> _enabled_extensions;
    static ValidationFeatures        _validation_features;
    static vk::ValidationFeaturesEXT _validation_extensions;

    static vk::InstanceCreateInfo _instance_create_info;

    static void _init_dynamic_loader();
    static void _init_app_info();
    static void _init_layers();
    static void _init_extensions();
};

} // namespace vkl
#endif // VKLEARNIN_RENDERING_GRAPHICSINSTANCE_HPP