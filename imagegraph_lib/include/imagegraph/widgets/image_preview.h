#pragma once

#include <imgui.h>

namespace imagegraph::widgets {
    void image_preview(ImTextureRef texture, ImVec2 texture_size, ImVec2 widget_size = ImVec2(200.0f, 150.0f));
}
