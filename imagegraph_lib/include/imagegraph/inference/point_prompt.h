#pragma once

#include <array>

namespace imagegraph::inference {
    enum class PointType {
        Positive,
        Negative,
    };

    struct PointPrompt {
        std::array<float, 2> position;
        PointType type;
    };
} // namespace imagegraph::inference
