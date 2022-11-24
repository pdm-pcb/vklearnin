#ifndef VKLEARNIN_ENGINE_FRAMEDATA_HPP
#define VKLEARNIN_ENGINE_FRAMEDATA_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/devices/CmdPool.hpp"
#include "vklearnin/rendering/devices/CmdBuffer.hpp"
#include "vklearnin/shaders/DescriptorPool.hpp"
#include "vklearnin/shaders/DescriptorSetLayout.hpp"
#include "vklearnin/shaders/DescriptorSet.hpp"

namespace vkl {

struct InstanceData;

class FrameData {
public:
    using Bindings = std::vector<vk::DescriptorSetLayoutBinding>;
    using PoolSizes = std::vector<vk::DescriptorPoolSize>;

    void image_changed();
    void update_instance_data(const InstanceData &data);

    void create(const Bindings &bindings);
    void destroy();

    inline const auto & command_pool()   const { return _command_pool; }
    inline const auto & command_buffer() const { return _command_buffer; }
    inline const auto & descriptor_set_layout() const {
        return _descriptor_set_layout;
    }
    inline const auto & descriptor_set() const { return _descriptor_set; }

    FrameData() = default;
    ~FrameData() = default;

    FrameData(FrameData &&other);
    FrameData(const FrameData &other) = delete;

    FrameData & operator=(FrameData &&other) = delete;
    FrameData & operator=(const FrameData &other) = delete;

private:
    CmdPool   _command_pool;
    CmdBuffer _command_buffer;

    DescriptorPool      _descriptor_pool;
    DescriptorSetLayout _descriptor_set_layout;
    DescriptorSet       _descriptor_set;
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_FRAMEDATA_HPP