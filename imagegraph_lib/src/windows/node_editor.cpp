#include <imagegraph/windows/node_editor.h>

#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace imagegraph {
    NodeEditor::NodeEditor(graph::Graph* graph) : _graph(graph), _navigate_to_content(false) {
        auto config = ax::NodeEditor::Config();
        config.SettingsFile = "node_editor.json";
        _context = ax::NodeEditor::CreateEditor(&config);

        if (auto file = std::ifstream("nodes.json")) {
            try {
                auto json = nlohmann::json();
                file >> json;
                deserialize(json);
            } catch (const std::exception& exception) {
                std::fprintf(stderr, "failed to load nodes: %s\n", exception.what());
            }
        }
    }

    NodeEditor::~NodeEditor() {
        try {
            const auto json = serialize();

            auto file = std::ofstream("nodes.json");
            if (!file) {
                throw std::runtime_error("failed to open nodes.json");
            }

            file << json.dump(4);
        } catch (const std::exception& exception) {
            std::fprintf(stderr, "failed to save nodes: %s\n", exception.what());
        }

        ax::NodeEditor::DestroyEditor(_context);
    }

    void NodeEditor::draw() {
        ax::NodeEditor::SetCurrentEditor(_context);
        ax::NodeEditor::Begin("Node Editor");

        for (const auto node: _graph->nodes()) {
            node->draw();
        }

        for (const auto node: _graph->nodes()) {
            for (const auto& input_pin: node->input_pins()) {
                if (input_pin.output_pin()) {
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

        if (_navigate_to_content) {
            _navigate_to_content = false;
            ax::NodeEditor::NavigateToContent();
        }

        ax::NodeEditor::End();
        ax::NodeEditor::SetCurrentEditor(nullptr);
    }

    nlohmann::json NodeEditor::serialize() const {
        ax::NodeEditor::SetCurrentEditor(_context);

        auto json = nlohmann::json();

        auto node_ids = std::unordered_map<graph::Node*, int>();
        node_ids.reserve(_graph->nodes().size());

        int id = 0;
        for (const auto node: _graph->nodes()) {
            node_ids[node] = id++;

            auto type = _registry.type(*node);
            auto position = ax::NodeEditor::GetNodePosition(node->id());

            json["nodes"].push_back({{"id", node_ids[node]},
                                     {"type", type},
                                     {"x", position.x},
                                     {"y", position.y},
                                     {"data", node->serialize()}});
        }

        for (const auto node: _graph->nodes()) {
            auto input_pins = node->input_pins();
            for (int input_index = 0; input_index < input_pins.size(); ++input_index) {
                auto& input_pin = input_pins[input_index];

                const auto output_pin = input_pin.output_pin();
                if (!output_pin) {
                    continue;
                }

                auto output_node = output_pin->owner();
                auto output_index = static_cast<int>(output_pin - output_node->output_pins().data());

                json["links"].push_back({{"from_node", node_ids[output_node]},
                                         {"from_pin", output_index},
                                         {"to_node", node_ids[node]},
                                         {"to_pin", input_index}});
            }
        }

        ax::NodeEditor::SetCurrentEditor(nullptr);
        return json;
    }

    void NodeEditor::deserialize(const nlohmann::json& json) {
        if (!json.contains("nodes") || !json["nodes"].is_array()) {
            return;
        }

        _graph->clear_nodes();
        _navigate_to_content = true;
        auto id_map = std::unordered_map<int, graph::Node*>();

        ax::NodeEditor::SetCurrentEditor(_context);
        for (auto& json_node: json["nodes"]) {
            if (!json_node.contains("type")) {
                continue;
            }

            const auto& type = json_node["type"];
            if (!type.is_string()) {
                continue;
            }

            const auto type_str = type.get<std::string>();
            auto node = _registry.create(type_str.c_str());

            if (!node) {
                continue;
            }

            if (json_node.contains("x") && json_node.contains("y") && json_node["x"].is_number() &&
                json_node["y"].is_number()) {
                auto position = ImVec2(json_node["x"].get<float>(), json_node["y"].get<float>());
                ax::NodeEditor::SetNodePosition(node->id(), position);
            }

            if (json_node.contains("data") && json_node["data"].is_object()) {
                node->deserialize(json_node["data"]);
            }

            const auto node_ptr = _graph->add_node(std::move(node));

            if (json_node.contains("id") && json_node["id"].is_number_integer()) {
                id_map.emplace(json_node["id"].get<int>(), node_ptr);
            }
        }
        ax::NodeEditor::SetCurrentEditor(nullptr);

        if (!json.contains("links") || !json["links"].is_array()) {
            return;
        }

        for (auto& link: json["links"]) {
            if (!link.contains("from_node") || !link.contains("to_node") || !link.contains("from_pin") ||
                !link.contains("to_pin")) {
                continue;
            }

            if (!link["from_node"].is_number_integer() || !link["to_node"].is_number_integer() ||
                !link["from_pin"].is_number_unsigned() || !link["to_pin"].is_number_unsigned()) {
                continue;
            }

            const auto from_node = id_map[link["from_node"].get<int>()];
            const auto to_node = id_map[link["to_node"].get<int>()];
            if (!from_node || !to_node) {
                continue;
            }

            const auto from_pin = link["from_pin"].get<std::size_t>();
            const auto to_pin = link["to_pin"].get<std::size_t>();
            if (from_pin >= from_node->output_pins().size() || to_pin >= to_node->input_pins().size()) {
                continue;
            }

            const auto output_pin = &from_node->output_pins()[from_pin];
            const auto input_pin = &to_node->input_pins()[to_pin];
            _graph->add_link(output_pin, input_pin);
        }
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
            for (const auto category: _registry.categories()) {
                if (!ImGui::BeginMenu(category)) {
                    continue;
                }

                for (const auto item: _registry.category_items(category)) {
                    if (!item) {
                        ImGui::Separator();
                    } else if (ImGui::MenuItem(_registry.label(item))) {
                        const auto node = _graph->add_node(_registry.create(item));
                        ax::NodeEditor::SetNodePosition(node->id(), open_popup_position);
                    }
                }

                ImGui::EndMenu();
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
