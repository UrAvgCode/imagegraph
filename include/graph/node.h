#pragma once

#include <graph/pin.h>

#include <imgui_node_editor.h>

#include <nlohmann/json.hpp>

#include <span>
#include <vector>

namespace imagegraph::graph {
    class Node {
    public:
        explicit Node();
        virtual ~Node() = default;

        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        Node(Node&&) = delete;
        Node& operator=(Node&&) = delete;

        virtual void draw() = 0;
        virtual void evaluate() = 0;

        virtual nlohmann::json serialize() const = 0;
        virtual void deserialize(const nlohmann::json &) = 0;

        void modified();

        ax::NodeEditor::NodeId id() const;
        std::span<InputPin> input_pins();
        std::span<OutputPin> output_pins();

    protected:
        ax::NodeEditor::NodeId _id;

        std::vector<InputPin> _input_pins;
        std::vector<OutputPin> _output_pins;

        bool _modified;
    };
} // namespace imagegraph::graph
