#define IMGUI_DEFINE_MATH_OPERATORS

#include <widgets/indeterminate_progress_bar.h>

#include <imgui_internal.h>

namespace imagegraph::widgets {
    void indeterminate_progress_bar(const bool loading, const ImVec2 size) {
        const auto window = ImGui::GetCurrentWindow();
        if (window->SkipItems) {
            return;
        }

        const auto style = ImGui::GetStyle();
        const auto padding = ImVec2(0.0f, style.FramePadding.y);

        const auto bounding_box = ImRect(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2.0f);
        ImGui::ItemSize(bounding_box);
        if (!ImGui::ItemAdd(bounding_box, 0)) {
            return;
        }

        const auto draw_list = window->DrawList;

        const auto bar_position = bounding_box.Min + padding;
        draw_list->AddRectFilled(bar_position, bar_position + size, ImGui::GetColorU32(ImGuiCol_FrameBg),
                                 size.y * 0.5f);

        if (!loading) {
            return;
        }

        const auto time = static_cast<float>(ImGui::GetTime());
        const float cycle = ImFmod(time, 1.0f);

        const float segment_width = size.x * 0.5f;
        const float segment_x0 = bar_position.x + (size.x + segment_width) * cycle - segment_width;
        const float segment_x1 = segment_x0 + segment_width;

        const float left = bar_position.x;
        const float right = bar_position.x + size.x;

        if (segment_x1 > left && segment_x0 < right) {
            const float clipped_segment_x0 = ImMax(segment_x0, left);
            const float clipped_segment_x1 = ImMin(segment_x1, right);

            draw_list->AddRectFilled(ImVec2(clipped_segment_x0, bar_position.y),
                                     ImVec2(clipped_segment_x1, bar_position.y + size.y),
                                     ImGui::GetColorU32(ImGuiCol_FrameBgActive), size.y * 0.5f);
        }
    }
} // namespace imagegraph::widgets
