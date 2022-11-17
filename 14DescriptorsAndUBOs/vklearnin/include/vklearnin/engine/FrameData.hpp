#ifndef VKLEARNIN_ENGINE_FRAMEDATA_HPP
#define VKLEARNIN_ENGINE_FRAMEDATA_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/devices/CmdPool.hpp"
#include "vklearnin/rendering/devices/CmdBuffer.hpp"

namespace vkl {

struct InstanceData;

class FrameData {
public:
    void image_changed();
    void update_instance_data(const InstanceData &data);

    void create();
    void destroy();

    inline const auto & cmd_pool()        const { return _cmd_pool; }
    inline const auto & cmd_buffer()      const { return _cmd_buffer; }
    inline const auto & dsc_set_layouts() const { return _set_layouts; }
    inline const auto & dsc_sets()        const { return _sets; }

    FrameData() = default;
    ~FrameData() = default;

    FrameData(FrameData &&other);
    FrameData(const FrameData &other) = delete;

    FrameData & operator=(FrameData &&other) = delete;
    FrameData & operator=(const FrameData &other) = delete;

private:
    CmdPool   _cmd_pool;
    CmdBuffer _cmd_buffer;

    BufferObject _ubo;

    vk::DescriptorPool _dsc_pool;

    std::vector<vk::DescriptorSetLayout> _set_layouts;
    std::vector<vk::DescriptorSet>       _sets;

    void _init_descriptors();
};

} // namespace vkl

#endif // VKLEARNIN_ENGINE_FRAMEDATA_HPP