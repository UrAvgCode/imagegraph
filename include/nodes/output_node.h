#pragma once

#include <graph/node.h>
#include <image/texture.h>

#include <string>

namespace imagegraph::nodes {
    class OutputNode final : public graph::Node {
    public:
        OutputNode();

        void draw() override;
        void evaluate() override;

        const std::string& name() const;
        const image::Texture* texture() const;

        void fit_to_canvas(ImVec2);
        bool consume_fit_request();

        void zoom_at(float, ImVec2);
        void pan_by(ImVec2);

        float zoom() const;
        ImVec2 pan() const;

    private:
        std::string _name;

        float _zoom;
        ImVec2 _pan;
        bool _fit_request;
    };
} // namespace imagegraph::nodes
