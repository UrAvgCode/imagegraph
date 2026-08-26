#pragma once

#include <imagegraph/compute/compute_program.h>
#include <imagegraph/compute/shader_storage_buffer.h>
#include <imagegraph/compute/texture.h>
#include <imagegraph/graph/node.h>

namespace imagegraph::nodes {
    class HistogramEqualizationNode final : public graph::Node {
    public:
        HistogramEqualizationNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        compute::Texture _texture;

        compute::ShaderStorageBuffer _histogram_buffer;
        compute::ShaderStorageBuffer _lookup_table_buffer;

        compute::ComputeProgram _histogram_program;
        compute::ComputeProgram _lookup_table_program;
        compute::ComputeProgram _histogram_equalization_program;
    };
} // namespace imagegraph::nodes
