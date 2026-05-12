#pragma once

#include <imgui_node_editor.h>

namespace imagegraph::graph {
    std::uint64_t generate_unique_id();

    ax::NodeEditor::NodeId generate_unique_node_id();
    ax::NodeEditor::PinId generate_unique_pin_id();
    ax::NodeEditor::LinkId generate_unique_link_id();
} // namespace imagegraph::graph
