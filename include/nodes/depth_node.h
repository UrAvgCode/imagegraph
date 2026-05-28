#pragma once

#include <compute/mask.h>
#include <graph/node.h>
#include <image/image.h>

#include <onnxruntime_cxx_api.h>

#include <array>
#include <future>

namespace imagegraph::nodes {
    class DepthNode final : public graph::Node {
    public:
        DepthNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        std::array<int, 2> _output_size;

        compute::Mask _mask;

        Ort::Env _env;
        Ort::Session _session;

        std::string _input_name;
        std::string _output_name;

        std::future<image::Image> _future;
        bool _processing;
    };
} // namespace imagegraph::nodes
