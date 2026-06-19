#include <imagegraph/graph/graph.h>

#include <cassert>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace {
    bool creates_cycle(const imagegraph::graph::Node* from, imagegraph::graph::Node* to) {
        if (from == to) {
            return true;
        }

        auto visited = std::unordered_set<imagegraph::graph::Node*>();
        auto queue = std::queue<imagegraph::graph::Node*>();
        queue.push(to);

        while (!queue.empty()) {
            auto node = queue.front();
            queue.pop();

            if (node == from) {
                return true;
            }

            if (!visited.insert(node).second) {
                continue;
            }

            for (const auto& output_pin: node->output_pins()) {
                for (const auto& input_pin: output_pin.connections()) {
                    queue.push(input_pin->owner());
                }
            }
        }

        return false;
    }
} // namespace

namespace imagegraph::graph {
    Graph::Graph() = default;

    Node* Graph::add_node(std::unique_ptr<Node> node) {
        _nodes.push_back(std::move(node));
        topological_sort_nodes();
        return _nodes.back().get();
    }

    void Graph::remove_node(const ax::NodeEditor::NodeId node_id) {
        const auto iterator =
                std::ranges::find_if(_nodes, [node_id](const auto& node) { return node->id() == node_id; });

        if (iterator == _nodes.end()) {
            return;
        }

        const auto node = iterator->get();

        for (const auto& output_pin: node->output_pins()) {
            for (const auto& input_pin: output_pin.connections()) {
                input_pin->disconnect();
            }
        }

        for (auto& input_pin: node->input_pins()) {
            input_pin.disconnect();
        }

        _nodes.erase(iterator);
        topological_sort_nodes();
    }

    void Graph::clear_nodes() {
        _sorted_nodes.clear();
        _nodes.clear();
    }

    void Graph::add_link(OutputPin* output_pin, InputPin* input_pin) {
        assert(validate_link(output_pin, input_pin));
        input_pin->connect(output_pin);
        topological_sort_nodes();
    }

    void Graph::remove_link(const ax::NodeEditor::LinkId link_id) {
        for (const auto& node: _nodes) {
            for (auto& input_pin: node->input_pins()) {
                if (input_pin.link_id() == link_id) {
                    input_pin.disconnect();
                }
            }
        }
        topological_sort_nodes();
    }

    bool Graph::validate_link(const OutputPin* output_pin, const InputPin* input_pin) {
        if (!output_pin || !input_pin) {
            return false;
        }

        if (output_pin->type() != input_pin->type()) {
            return false;
        }

        if (creates_cycle(output_pin->owner(), input_pin->owner())) {
            return false;
        }

        return true;
    }

    void Graph::evaluate() const {
        for (const auto node: _sorted_nodes) {
            node->evaluate();
        }
    }

    std::span<Node* const> Graph::nodes() const { return _sorted_nodes; }

    Pin* Graph::pin(const ax::NodeEditor::PinId id) const {
        if (!id) {
            return nullptr;
        }

        for (const auto node: _sorted_nodes) {
            for (auto& pin: node->input_pins()) {
                if (pin.id() == id) {
                    return &pin;
                }
            }

            for (auto& pin: node->output_pins()) {
                if (pin.id() == id) {
                    return &pin;
                }
            }
        }

        return nullptr;
    }

    void Graph::topological_sort_nodes() {
        _sorted_nodes.clear();
        _sorted_nodes.reserve(_nodes.size());

        auto indegree = std::unordered_map<Node*, int>();
        indegree.reserve(_nodes.size());

        for (auto& node: _nodes) {
            indegree[node.get()] = 0;
        }

        for (const auto& node: _nodes) {
            for (const auto& input_pin: node->input_pins()) {
                if (input_pin.output_pin()) {
                    indegree[node.get()]++;
                }
            }
        }

        auto queue = std::queue<Node*>();
        for (auto& [node, deg]: indegree) {
            if (deg == 0) {
                queue.push(node);
            }
        }

        while (!queue.empty()) {
            auto node = queue.front();
            queue.pop();

            _sorted_nodes.push_back(node);

            for (const auto& output_pin: node->output_pins()) {
                for (const auto& input_pin: output_pin.connections()) {
                    const auto owner = input_pin->owner();
                    if (--indegree[owner] == 0) {
                        queue.push(owner);
                    }
                }
            }
        }

        assert(_sorted_nodes.size() == _nodes.size());
    }
} // namespace imagegraph::graph
