#ifndef VKLEARNIN_RENDER_RENDERPASS_CMDPOOL_HPP
#define VKLEARNIN_RENDER_RENDERPASS_CMDPOOL_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/render/renderpass/CmdBuffer.hpp"

namespace vkl {

class CmdPool final {
public:
    using Buffers = std::vector<CmdBuffer>;
    using BufferIter = Buffers::iterator;

    void create();
    void destroy();

    BufferIter allocate_buffer();

    void reset(const vk::CommandPoolResetFlags flags = { }) const;

    inline auto & native() const { return _pool; }

    CmdPool();
    ~CmdPool() = default;

    CmdPool(CmdPool &&other) noexcept;
    CmdPool(const CmdPool &) = delete;

    CmdPool & operator=(CmdPool &&) = delete;
    CmdPool & operator=(const CmdPool &) = delete;

private:
    vk::CommandPool _pool;
    Buffers _buffers;
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_RENDERPASS_CMDPOOL_HPP