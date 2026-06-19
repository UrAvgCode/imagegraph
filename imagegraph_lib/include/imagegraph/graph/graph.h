#pragma once

#include <imagegraph/graph/node.h>

#include <memory>
#include <span>
#include <vector>

namespace imagegraph::graph {
    class Graph {
    public:
        Graph();

        Node* add_node(std::unique_ptr<Node>);
        void remove_node(ax::NodeEditor::NodeId);
        void clear_nodes();

        void add_link(OutputPin*, InputPin*);
        void remove_link(ax::NodeEditor::LinkId);
        static bool validate_link(const OutputPin*, const InputPin*);

        void evaluate() const;

        std::span<Node* const> nodes() const;
        Pin* pin(ax::NodeEditor::PinId) const;

    private:
        void topological_sort_nodes();

        std::vector<std::unique_ptr<Node>> _nodes;
        std::vector<Node*> _sorted_nodes;
    };
} // namespace imagegraph::graph
