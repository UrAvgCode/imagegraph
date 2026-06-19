#pragma once

#include <windows/node_editor.h>

namespace imagegraph {
    class MainMenuBar {
    public:
        explicit MainMenuBar(NodeEditor*);

        void draw() const;

    private:
        void handle_shortcuts() const;

        void open() const;
        void save() const;

        NodeEditor* _node_editor;
    };
} // namespace imagegraph
