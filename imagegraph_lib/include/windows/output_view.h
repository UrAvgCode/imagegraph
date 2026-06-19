#pragma once

#include <graph/graph.h>

namespace imagegraph {
    class OutputView {
    public:
        explicit OutputView(graph::Graph*);

        void draw() const;

    private:
        graph::Graph* _graph;
    };
} // namespace imagegraph
