#include <imagegraph/nodes/registry.h>

#include <imagegraph/nodes/input_node.h>
#include <imagegraph/nodes/output_node.h>

#include <imagegraph/nodes/brightness_contrast_node.h>
#include <imagegraph/nodes/color_intensity_node.h>
#include <imagegraph/nodes/histogram_equalization_node.h>
#include <imagegraph/nodes/white_balance_node.h>

#include <imagegraph/nodes/blend_node.h>
#include <imagegraph/nodes/bokeh_node.h>
#include <imagegraph/nodes/gaussian_blur_node.h>
#include <imagegraph/nodes/median_node.h>

#include <imagegraph/nodes/color_key_node.h>
#include <imagegraph/nodes/depth_node.h>
#include <imagegraph/nodes/mask_gaussian_blur_node.h>
#include <imagegraph/nodes/mask_grow_shrink_node.h>
#include <imagegraph/nodes/mask_invert_node.h>
#include <imagegraph/nodes/segment_node.h>

namespace imagegraph::nodes {
    template<typename NodeType>
    void Registry::register_node(const char* type, const char* label, const char* category) {
        static_assert(std::derived_from<NodeType, graph::Node>);

        auto entry = Entry{.label = label,
                           .factory = []() -> std::unique_ptr<graph::Node> { return std::make_unique<NodeType>(); }};

        _entries.emplace(type, entry);
        _types.emplace(typeid(NodeType), type);

        if (!_category_types.contains(category)) {
            _categories.emplace_back(category);
        }

        _category_types[category].emplace_back(type);
    }

    Registry::Registry() {
        register_node<InputNode>("input", "Input", "Input/Output");
        register_node<OutputNode>("output", "Output", "Input/Output");

        register_node<BrightnessContrastNode>("brightness_contrast", "Brightness/Contrast", "Color");
        register_node<ColorIntensityNode>("color_intensity", "Color Intensity", "Color");
        register_node<WhiteBalanceNode>("white_balance", "White Balance", "Color");
        register_node<HistogramEqualizationNode>("histogram_equalization", "Histogram Equalization", "Color");

        register_node<GaussianBlurNode>("gaussian_blur", "Gaussian Blur", "Effects");
        register_node<MedianNode>("median", "Median", "Effects");
        register_node<BokehNode>("bokeh", "Bokeh", "Effects");
        register_node<BlendNode>("blend", "Blend", "Effects");

        register_node<DepthNode>("depth", "Depth", "Masks");
        register_node<SegmentNode>("segment", "Segment", "Masks");
        register_node<ColorKeyNode>("color_key", "Color Key", "Masks");
        register_node<MaskInvertNode>("mask_invert", "Invert Mask", "Masks");
        register_node<MaskGrowShrinkNode>("mask_grow_shrink", "Grow/Shrink Mask", "Masks");
        register_node<MaskGaussianBlurNode>("mask_gaussian_blur", "Mask Gaussian Blur", "Masks");
    }

    std::unique_ptr<graph::Node> Registry::create(const char* type) const { return _entries.at(type).factory(); }

    const char* Registry::type(const graph::Node& node) const { return _types.at(std::type_index(typeid(node))); }

    const char* Registry::label(const char* type) const { return _entries.at(type).label; }

    std::span<const char* const> Registry::categories() const { return _categories; }

    std::span<const char* const> Registry::category_types(const char* category) const {
        return _category_types.at(category);
    }
} // namespace imagegraph::nodes
