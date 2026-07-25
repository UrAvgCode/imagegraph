#pragma once

#include <imagegraph/inference/point_prompt.h>

#include <imgui.h>

#include <vector>

namespace imagegraph::widgets {
    void image_preview(ImTextureRef texture, ImVec2 texture_size, ImVec2 widget_size = ImVec2(200.0f, 150.0f));

    bool prompt_image_preview(ImTextureRef texture, ImVec2 texture_size, ImVec2 widget_size,
                              std::vector<inference::PointPrompt>& prompts);
} // namespace imagegraph::widgets
