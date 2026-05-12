#include <windows/node_editor.h>

#include <nodes/input_node.h>
#include <nodes/output_node.h>

namespace imagegraph {
    NodeEditor::NodeEditor(graph::Graph* graph) : _graph(graph) {
        auto config = ax::NodeEditor::Config();
        config.SettingsFile = "node_editor.json";
        _context = ax::NodeEditor::CreateEditor(&config);
    }

    NodeEditor::~NodeEditor() { ax::NodeEditor::DestroyEditor(_context); }

    void NodeEditor::draw() const {
        ax::NodeEditor::SetCurrentEditor(_context);
        ax::NodeEditor::Begin("Node Editor");

        for (const auto node: _graph->nodes()) {
            node->draw();
        }

        for (const auto node: _graph->nodes()) {
            for (const auto& input_pin: node->input_pins()) {
                if (input_pin.is_connected()) {
                    const auto link_id = input_pin.link_id();
                    const auto input_pin_id = input_pin.id();
                    const auto output_pin_id = input_pin.output_pin()->id();
                    ax::NodeEditor::Link(link_id, output_pin_id, input_pin_id);
                }
            }
        }

        handle_creation_action();
        handle_node_creation_popup();
        handle_deletion_action();

        ax::NodeEditor::End();
        ax::NodeEditor::SetCurrentEditor(nullptr);
    }

    void NodeEditor::handle_creation_action() const {
        if (ax::NodeEditor::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
            auto start_pin_id = ax::NodeEditor::PinId();
            auto end_pin_id = ax::NodeEditor::PinId();

            if (ax::NodeEditor::QueryNewLink(&start_pin_id, &end_pin_id)) {
                auto start_pin = _graph->pin(start_pin_id);
                auto end_pin = _graph->pin(end_pin_id);

                if (start_pin->kind() == ax::NodeEditor::PinKind::Input) {
                    std::swap(start_pin, end_pin);
                    std::swap(start_pin_id, end_pin_id);
                }

                if (end_pin && start_pin) {
                    const auto output_pin = dynamic_cast<graph::OutputPin*>(start_pin);
                    const auto input_pin = dynamic_cast<graph::InputPin*>(end_pin);

                    if (!graph::Graph::validate_link(output_pin, input_pin)) {
                        ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 2.0f);
                    } else if (ax::NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                        _graph->add_link(output_pin, input_pin);
                    }
                }
            }
        }
        ax::NodeEditor::EndCreate();
    }

    void NodeEditor::handle_node_creation_popup() const {
        const auto open_popup_position = ImGui::GetMousePos();
        ax::NodeEditor::Suspend();
        if (ax::NodeEditor::ShowBackgroundContextMenu()) {
            ImGui::OpenPopup("Create New Node");
        }
        ax::NodeEditor::Resume();

        ax::NodeEditor::Suspend();
        if (ImGui::BeginPopup("Create New Node")) {
            const graph::Node* new_node = nullptr;

            if (ImGui::MenuItem("Input")) {
                new_node = _graph->add_node(std::make_unique<nodes::InputNode>());
            } else if (ImGui::MenuItem("Output")) {
                new_node = _graph->add_node(std::make_unique<nodes::OutputNode>());
            }

            if (new_node) {
                ax::NodeEditor::SetNodePosition(new_node->id(), open_popup_position);

                ax::NodeEditor::GetNodePosition(new_node->id());
            }

            ImGui::EndPopup();
        }
        ax::NodeEditor::Resume();
    }

    void NodeEditor::handle_deletion_action() const {
        if (ax::NodeEditor::BeginDelete()) {
            auto link_id = ax::NodeEditor::LinkId(0);
            while (ax::NodeEditor::QueryDeletedLink(&link_id)) {
                if (ax::NodeEditor::AcceptDeletedItem()) {
                    _graph->remove_link(link_id);
                }
            }

            auto node_id = ax::NodeEditor::NodeId(0);
            while (ax::NodeEditor::QueryDeletedNode(&node_id)) {
                if (ax::NodeEditor::AcceptDeletedItem()) {
                    _graph->remove_node(node_id);
                }
            }
        }
        ax::NodeEditor::EndDelete();
    }
} // namespace imagegraph
