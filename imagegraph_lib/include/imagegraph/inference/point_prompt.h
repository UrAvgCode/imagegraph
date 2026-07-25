#pragma once

#include <array>

namespace imagegraph::inference {
    enum class PointLabel {
        Foreground,
        Background,
    };

    struct PointPrompt {
        std::array<float, 2> position;
        PointLabel label;
    };
} // namespace imagegraph::inference
