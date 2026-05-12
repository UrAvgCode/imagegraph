#include <graph/ids.h>

namespace imagegraph::graph {
    static std::uint64_t next_id = 0;

    std::uint64_t generate_unique_id() { return ++next_id; }

    ax::NodeEditor::NodeId generate_unique_node_id() { return generate_unique_id(); }

    ax::NodeEditor::PinId generate_unique_pin_id() { return generate_unique_id(); }

    ax::NodeEditor::LinkId generate_unique_link_id() { return generate_unique_id(); }
} // namespace imagegraph::graph
