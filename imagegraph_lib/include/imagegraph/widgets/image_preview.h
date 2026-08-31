#pragma once

#include <imagegraph/compute/mask.h>
#include <imagegraph/compute/texture.h>
#include <imagegraph/inference/point_prompt.h>

#include <imgui.h>

#include <vector>

namespace imagegraph::widgets {
    void image_preview(ImTextureRef, ImVec2 texture_size, ImVec2 widget_size = ImVec2(200.0f, 150.0f));
    void image_preview(const compute::Texture&, ImVec2 widget_size = ImVec2(200.0f, 150.0f));
    void image_preview(const compute::Mask&, ImVec2 widget_size = ImVec2(200.0f, 150.0f));

    bool prompt_image_preview(ImTextureRef texture, ImVec2 texture_size, ImVec2 widget_size,
                              std::vector<inference::PointPrompt>& prompts);
} // namespace imagegraph::widgets
