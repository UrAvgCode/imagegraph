#define IMGUI_DEFINE_MATH_OPERATORS

#include <imagegraph/widgets/image_preview.h>

#include <imgui_internal.h>

namespace {
    constexpr auto point_radius = 2.5f;
    constexpr auto outline_width = 1.0f;
    constexpr auto outline_radius = point_radius + outline_width;

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
        if (!ImGui::ItemAdd(bounding_box, 0)) {
            return;
        }

        const auto draw_list = window->DrawList;

        const auto background_position = bounding_box.Min + padding;
        const auto background_max = background_position + widget_size;
        draw_list->PushClipRect(background_position, background_max, true);
        checkerboard_background(draw_list, background_position, widget_size);

        if (texture.GetTexID() && texture_size.x > 0.0f && texture_size.y > 0.0f) {
            const auto ratio = widget_size / texture_size;
            const auto scale = ImMin(ratio.x, ratio.y);

            const auto image_size = texture_size * scale;
            const auto offset = (widget_size - image_size) * 0.5f;

            const auto image_min = background_position + offset;
            const auto image_max = image_min + image_size;

            draw_list->AddImage(texture, image_min, image_max);
        }

        draw_list->PopClipRect();
    }

    bool prompt_image_preview(const ImTextureRef texture, const ImVec2 texture_size, const ImVec2 widget_size,
                              std::vector<inference::PointPrompt>& prompts) {
        const auto window = ImGui::GetCurrentWindow();
        if (window->SkipItems) {
            return false;
        }

        const auto style = ImGui::GetStyle();
        const auto padding = ImVec2(style.ImageBorderSize, style.ImageBorderSize);

        const auto widget_id = window->GetID("##prompt_image_preview");
        const auto bounding_box = ImRect(window->DC.CursorPos, window->DC.CursorPos + widget_size + padding * 2.0f);
        ImGui::ItemSize(bounding_box);
        if (!ImGui::ItemAdd(bounding_box, widget_id)) {
            return false;
        }

        const auto draw_list = window->DrawList;

        const auto background_position = bounding_box.Min + padding;
        const auto background_max = background_position + widget_size;
        draw_list->PushClipRect(background_position, background_max, true);
        checkerboard_background(draw_list, background_position, widget_size);

        if (!texture.GetTexID() || texture_size.x <= 0.0f || texture_size.y <= 0.0f) {
            draw_list->PopClipRect();
            return false;
        }

        const auto ratio = widget_size / texture_size;
        const auto scale = ImMin(ratio.x, ratio.y);

        const auto image_size = texture_size * scale;
        const auto offset = (widget_size - image_size) * 0.5f;

        const auto image_min = background_position + offset;
        const auto image_max = image_min + image_size;
        const auto image_rect = ImRect{image_min, image_max};

        draw_list->AddImage(texture, image_min, image_max);

        bool held = false;
        bool hovered = false;
        constexpr auto flags = ImGuiButtonFlags{ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight |
                                                ImGuiButtonFlags_PressedOnClick};

        bool prompts_changed = false;
        if (ImGui::ButtonBehavior(image_rect, widget_id, &hovered, &held, flags)) {
            const auto mouse_position = ImGui::GetIO().MousePos;

            if (ImGui::GetIO().KeyShift && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                constexpr float hit_radius_squared = outline_radius * outline_radius;

                for (auto i = static_cast<std::ptrdiff_t>(prompts.size()) - 1; i >= 0; --i) {
                    const auto& [position, label] = prompts[i];

                    const auto image_position = ImVec2{position[0] * texture_size.x, position[1] * texture_size.y};
                    const auto circle_position = image_min + image_position * scale;

                    const auto delta = mouse_position - circle_position;
                    const auto distance_squared = delta.x * delta.x + delta.y * delta.y;

                    if (distance_squared <= hit_radius_squared) {
                        prompts.erase(prompts.begin() + i);
                        prompts_changed = true;
                        break;
                    }
                }
            } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                auto point = (mouse_position - image_min) / scale;
                point.x = ImClamp(point.x, 0.0f, texture_size.x);
                point.y = ImClamp(point.y, 0.0f, texture_size.y);

                prompts.push_back(
                        {{point.x / texture_size.x, point.y / texture_size.y}, inference::PointType::Positive});
                prompts_changed = true;
            } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                auto point = (mouse_position - image_min) / scale;
                point.x = ImClamp(point.x, 0.0f, texture_size.x);
                point.y = ImClamp(point.y, 0.0f, texture_size.y);

                prompts.push_back(
                        {{point.x / texture_size.x, point.y / texture_size.y}, inference::PointType::Negative});
                prompts_changed = true;
            }
        }

        for (const auto& [position, label]: prompts) {
            if (position[0] < 0.0f || position[1] < 0.0f || position[0] > 1.0f || position[1] > 1.0f) {
                continue;
            }

            const auto image_position = ImVec2{position[0] * texture_size.x, position[1] * texture_size.y};
            const auto screen_position = image_min + image_position * scale;

            constexpr auto positive_color = IM_COL32(50, 220, 100, 255);
            constexpr auto negative_color = IM_COL32(255, 80, 80, 255);
            constexpr auto outline_color = IM_COL32(20, 20, 20, 220);

            const auto fill_color = label == inference::PointType::Positive ? positive_color : negative_color;
            draw_list->AddCircleFilled(screen_position, outline_radius, outline_color, 24);
            draw_list->AddCircleFilled(screen_position, point_radius, fill_color, 20);
        }

        draw_list->PopClipRect();
        return prompts_changed;
    }
} // namespace imagegraph::widgets
