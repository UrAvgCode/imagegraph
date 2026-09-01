#pragma once

#include <imagegraph/windows/node_editor.h>

namespace imagegraph {
    class MainMenuBar {
    public:
        explicit MainMenuBar(NodeEditor*);

        void draw();

    private:
        void handle_shortcuts();
        void draw_help_window();

        void open() const;
        void save() const;

        NodeEditor* _node_editor;

        bool _show_help;
        bool _show_metrics;
    };
} // namespace imagegraph
