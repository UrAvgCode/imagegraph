#include <imagegraph/nodes/mask_gaussian_blur_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/mask_gaussian_blur.h>

namespace imagegraph::nodes {
    MaskGaussianBlurNode::MaskGaussianBlurNode() : MaskGaussianBlurNode(3, 3) {}

    MaskGaussianBlurNode::MaskGaussianBlurNode(const int size_x, const int size_y) : _blur_size(size_x, size_y) {
        _input_pins.emplace_back(graph::Pin::Type::Mask, this);
        _output_pins.emplace_back(graph::Pin::Type::Mask, this);

        _compute_program.load(shader::mask_gaussian_blur);
    }

    void MaskGaussianBlurNode::draw() {
        begin_node("Mask Gaussian Blur");

        constexpr float total_width = 200.0f;
        constexpr float label_width = 40.0f;
        constexpr float input_width = total_width - label_width;

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Size");

        ImGui::SameLine(label_width);
        ImGui::SetNextItemWidth(input_width);
        if (ImGui::DragInt2("##size", &_blur_size.x, 0.2, 0, 150)) {
            modified();
        }

        widgets::image_preview(_mask);

        end_node();
    }

    void MaskGaussianBlurNode::evaluate() {
        if (!_modified) {
            return;
        }
        _modified = false;

        const auto input_mask = _input_pins[0].mask();
        if (!input_mask || input_mask->id() == 0) {
            _mask = compute::Mask();
            _output_pins[0].set_mask(nullptr);
            return;
        }

        const auto width = input_mask->width();
        const auto height = input_mask->height();
        if (width == 0 || height == 0) {
            _mask = compute::Mask();
            _output_pins[0].set_mask(nullptr);
            return;
        }

        _mask.allocate(width, height);
        _temp_mask.allocate(width, height);

        input_mask->bind_image(0, GL_READ_ONLY);
        _temp_mask.bind_image(1, GL_WRITE_ONLY);

        _compute_program.set_uniform_ivec2(0, _blur_size);
        _compute_program.set_uniform_int(1, false);
        _compute_program.dispatch(width, height);

        _temp_mask.bind_image(0, GL_READ_ONLY);
        _mask.bind_image(1, GL_WRITE_ONLY);

        _compute_program.set_uniform_int(1, true);
        _compute_program.dispatch(width, height);

        _output_pins[0].set_mask(&_mask);
    }

    nlohmann::json MaskGaussianBlurNode::serialize() const {
        return {{"size_x", _blur_size.x}, {"size_y", _blur_size.y}};
    }

    void MaskGaussianBlurNode::deserialize(const nlohmann::json& json) {
        if (json.contains("size_x") && json["size_x"].is_number()) {
            _blur_size.x = json["size_x"].get<int>();
        }
        if (json.contains("size_y") && json["size_y"].is_number()) {
            _blur_size.y = json["size_y"].get<int>();
        }
        modified();
    }
} // namespace imagegraph::nodes
