#include <windows/node_editor.h>

NodeEditor::NodeEditor() {
    auto config = ax::NodeEditor::Config();
    config.SettingsFile = "node_editor.json";
    _context = ax::NodeEditor::CreateEditor(&config);
}

NodeEditor::~NodeEditor() { ax::NodeEditor::DestroyEditor(_context); }

void NodeEditor::draw() const {
    ax::NodeEditor::SetCurrentEditor(_context);
    ax::NodeEditor::Begin("Node Editor");

    ax::NodeEditor::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);
}
