#ifndef VKLEARNIN_RENDERING_GRAPH_RENDERGRAPH_HPP
#define VKLEARNIN_RENDERING_GRAPH_RENDERGRAPH_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/rendering/graph/RenderGraphNode.hpp"

namespace vkl {

class RenderGraph {
public:
    inline void add_node(RenderGraphNode const &node) {
        _nodes.emplace_back(node);
    }

    RenderGraph();
    ~RenderGraph() = default;

    RenderGraph(RenderGraph &&) = delete;
    RenderGraph(const RenderGraph &) = delete;

    RenderGraph & operator=(RenderGraph &&) = delete;
    RenderGraph & operator=(const RenderGraph &) = delete;

private:
    std::vector<RenderGraphNode> _nodes;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_GRAPH_RENDERGRAPHNODE_HPP