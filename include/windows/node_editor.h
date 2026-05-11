#pragma once

#include <imgui_node_editor.h>

class NodeEditor {
public:
    explicit NodeEditor();
    ~NodeEditor();

    void draw() const;

private:
    ax::NodeEditor::EditorContext* _context;
};
