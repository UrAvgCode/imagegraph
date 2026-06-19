#include <imagegraph/windows/output_view.h>

#include <imagegraph/compute/transfer.h>
#include <imagegraph/image/io.h>
#include <imagegraph/nodes/output_node.h>
#include <imagegraph/platform/file_dialog.h>

#include <algorithm>
#include <cmath>
#include <utility>

namespace imagegraph {
    OutputView::OutputView(graph::Graph* graph) : _graph(graph) {}

    void OutputView::draw() const {
        if (ImGui::BeginTabBar("OutputTabs", ImGuiTabBarFlags_Reorderable)) {
            for (const auto node: _graph->nodes()) {
                const auto output_node = dynamic_cast<nodes::OutputNode*>(node);
                if (!output_node) {
                    continue;
                }

                ImGui::PushID(output_node);
                if (ImGui::BeginTabItem(output_node->name().c_str())) {
                    const auto texture = output_node->texture();

                    if (ImGui::Button("Save")) {
                        if (texture && texture->id()) {
                            auto path = platform::save_image_dialog();
                            if (!path.empty()) {
                                auto image = compute::download_texture(texture);
                                image::save_to_file_async(std::move(image), std::move(path));
                            }
                        }
                    }

                    if (texture && texture->id() != 0) {
                        const auto texture_width = static_cast<float>(texture->width());
                        const auto texture_height = static_cast<float>(texture->height());

                        const auto canvas_size = ImGui::GetContentRegionAvail();
                        const auto canvas_pos = ImGui::GetCursorScreenPos();

                        constexpr auto button_flags =
                                ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight;
                        ImGui::InvisibleButton("canvas", canvas_size, button_flags);

                        const bool hovered = ImGui::IsItemHovered();
                        const bool active = ImGui::IsItemActive();
                        const auto io = ImGui::GetIO();

                        if (hovered && io.MouseWheel != 0.0f) {
                            const float factor = std::pow(1.2f, io.MouseWheel);
                            const auto mouse_pos = ImVec2(io.MousePos.x - canvas_pos.x, io.MousePos.y - canvas_pos.y);
                            output_node->zoom_at(factor, mouse_pos);
                        }

                        if (active && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
                            output_node->pan_by(io.MouseDelta);
                        }

                        if (output_node->consume_fit_request() || (hovered && ImGui::IsKeyPressed(ImGuiKey_F))) {
                            output_node->fit_to_canvas(canvas_size);
                        }

                        const auto draw_list = ImGui::GetWindowDrawList();
                        const auto canvas_max = ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y);
                        draw_list->PushClipRect(canvas_pos, canvas_max, true);
                        {
                            draw_list->AddRectFilled(canvas_pos, canvas_max, IM_COL32(27, 27, 30, 255));

                            const auto zoom = output_node->zoom();
                            const auto pan = output_node->pan();

                            const auto image_min = ImVec2(canvas_pos.x + pan.x, canvas_pos.y + pan.y);
                            const auto image_max =
                                    ImVec2(image_min.x + texture_width * zoom, image_min.y + texture_height * zoom);

                            draw_list->AddImage(texture->id(), image_min, image_max);
                            draw_list->AddRect(canvas_pos, canvas_max, IM_COL32(255, 255, 255, 50));
                        }
                        draw_list->PopClipRect();
                    }

                    ImGui::EndTabItem();
                }

                ImGui::PopID();
            }

            ImGui::EndTabBar();
        }
    }
} // namespace imagegraph
