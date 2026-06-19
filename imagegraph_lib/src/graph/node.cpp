#include <imagegraph/graph/ids.h>
#include <imagegraph/graph/node.h>

namespace imagegraph::graph {
    Node::Node() : _id(generate_unique_node_id()), _modified(false) {}

    void Node::modified() { _modified = true; }

    ax::NodeEditor::NodeId Node::id() const { return _id; }

    std::span<InputPin> Node::input_pins() { return _input_pins; }

    std::span<OutputPin> Node::output_pins() { return _output_pins; }
} // namespace imagegraph::graph
