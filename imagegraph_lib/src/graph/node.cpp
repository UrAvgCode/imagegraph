#include <imagegraph/graph/ids.h>
#include <imagegraph/graph/node.h>

namespace imagegraph::graph {
    Node::Node() : _id(generate_unique_node_id()), _modified(false) {}

    void Node::modified() { _modified = true; }

    ax::NodeEditor::NodeId Node::id() const { return _id; }

    std::span<InputPin> Node::input_pins() { return _input_pins; }

    std::span<OutputPin> Node::output_pins() { return _output_pins; }

    void Node::begin_node(const char* title) const {
        ax::NodeEditor::BeginNode(_id);
        ImGui::PushID(_id.AsPointer());

        ImGui::TextUnformatted(title);

        if (!_input_pins.empty()) {
            ImGui::BeginGroup();
            for (const auto& pin: _input_pins) {
                pin.draw();
            }
            ImGui::EndGroup();
            ImGui::SameLine();
        }

        ImGui::BeginGroup();
    }

    void Node::end_node() const {
        ImGui::EndGroup();

        if (!_output_pins.empty()) {
            ImGui::SameLine();
            ImGui::BeginGroup();
            for (const auto& pin: _output_pins) {
                pin.draw();
            }
            ImGui::EndGroup();
        }

        ImGui::PopID();
        ax::NodeEditor::EndNode();
    }
} // namespace imagegraph::graph
