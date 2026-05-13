#pragma once

#include <graph/graph.h>

#include <imgui_node_editor.h>

namespace imagegraph {
    class NodeEditor {
    public:
        explicit NodeEditor(graph::Graph*);
        ~NodeEditor();

        void draw() const;

        nlohmann::json serialize() const;
        void deserialize(const nlohmann::json &) const;

    private:
        void handle_creation_action() const;
        void handle_node_creation_popup() const;
        void handle_deletion_action() const;

        ax::NodeEditor::EditorContext* _context;
        graph::Graph* _graph;
    };
} // namespace imagegraph
