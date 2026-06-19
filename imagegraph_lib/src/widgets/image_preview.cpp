#define IMGUI_DEFINE_MATH_OPERATORS

#include <imagegraph/widgets/image_preview.h>

#include <imgui_internal.h>

namespace {
    void checkerboard_background(ImDrawList* draw_list, const ImVec2 position, const ImVec2 size) {
        constexpr float cell = 10.0f;

        constexpr auto dark_gray = IM_COL32(60, 60, 60, 255);
        constexpr auto light_gray = IM_COL32(90, 90, 90, 255);

        const auto cols = static_cast<int>(ImCeil(size.x / cell));
        const auto rows = static_cast<int>(ImCeil(size.y / cell));

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                const auto color = (x + y) % 2 ? light_gray : dark_gray;

                const auto min = position + ImVec2(static_cast<float>(x), static_cast<float>(y)) * cell;
                const auto max = min + ImVec2(cell, cell);

                draw_list->AddRectFilled(min, max, color);
            }
        }
    }
} // namespace

namespace imagegraph::widgets {
    void image_preview(const ImTextureRef texture, const ImVec2 texture_size, const ImVec2 widget_size) {
        const auto window = ImGui::GetCurrentWindow();
        if (window->SkipItems) {
            return;
        }

        const auto style = ImGui::GetStyle();
        const auto padding = ImVec2(style.ImageBorderSize, style.ImageBorderSize);

        const auto bounding_box = ImRect(window->DC.CursorPos, window->DC.CursorPos + widget_size + padding * 2.0f);
        ImGui::ItemSize(bounding_box);
        if (!ImGui::ItemAdd(bounding_box, 0))
            return;

        const auto draw_list = window->DrawList;

        const auto background_position = bounding_box.Min + padding;
        checkerboard_background(draw_list, background_position, widget_size);

        if (texture._TexID && texture_size.x > 0 && texture_size.y > 0) {
            const auto ratio = widget_size / texture_size;
            const float scale = ImMin(ratio.x, ratio.y);

            const auto image_size = texture_size * scale;
            const auto offset = (widget_size - image_size) * 0.5f;

            const auto min = background_position + offset;
            const auto max = min + image_size;

            draw_list->AddImage(texture, min, max);
        }
    }
} // namespace imagegraph::widgets
