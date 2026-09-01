#include <imagegraph/windows/main_menu_bar.h>

#include <GLFW/glfw3.h>

#include <imagegraph/platform/file_dialog.h>

#include <cstdio>
#include <fstream>

namespace {
    void control_description(const char* control, const char* description) {
        constexpr float button_width = 140.0f;

        ImGui::BeginDisabled();
        ImGui::Button(control, ImVec2(button_width, 0.0f));
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::TextUnformatted(description);
    }
} // namespace

namespace imagegraph {
    MainMenuBar::MainMenuBar(NodeEditor* node_editor) :
        _node_editor(node_editor), _show_help(false), _show_metrics(false) {}

    void MainMenuBar::draw() {
        handle_shortcuts();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    open();
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    save();
                }

                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                ImGui::MenuItem("Controls", "F1", &_show_help);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Debug")) {
                ImGui::MenuItem("Metrics", "Ctrl+Shift+M", &_show_metrics);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (_show_help) {
            draw_help_window();
        }
        if (_show_metrics) {
            ImGui::ShowMetricsWindow(&_show_metrics);
        }
    }

    void MainMenuBar::draw_help_window() {
        if (!ImGui::Begin("Controls", &_show_help, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::End();
            return;
        }

        ImGui::SeparatorText("Node Editor");
        control_description("Mouse Wheel##node", "Zoom");
        control_description("Right Mouse Drag##node", "Pan");
        control_description("Right Mouse##node", "Open node menu");
        control_description("Del##node", "Delete selected item");
        control_description("F##node", "Center selected item");

        ImGui::SeparatorText("Output View");
        control_description("Mouse Wheel##output", "Zoom");
        control_description("Right Mouse Drag##output", "Pan");
        control_description("F##output", "Fit image to view");

        ImGui::End();
    }

    void MainMenuBar::handle_shortcuts() {
        if (ImGui::GetIO().WantTextInput) {
            return;
        }

        if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_O)) {
            open();
        }
        if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_S)) {
            save();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_F1)) {
            _show_help = !_show_help;
        }
        if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_M)) {
            _show_metrics = !_show_metrics;
        }
    }

    void MainMenuBar::open() const {
        auto path = platform::open_json_dialog();
        if (!path.empty()) {
            try {
                auto file = std::ifstream(path);
                if (!file) {
                    throw std::runtime_error("failed to open file");
                }

                auto json = nlohmann::json();
                file >> json;

                _node_editor->deserialize(json);
            } catch (const std::exception& exception) {
                std::fprintf(stderr, "failed to load: %s\n", exception.what());
            }
        }
    }

    void MainMenuBar::save() const {
        auto path = platform::save_json_dialog();
        if (!path.empty()) {
            try {
                auto json = _node_editor->serialize();

                auto file = std::ofstream(path);
                if (!file) {
                    throw std::runtime_error("failed to open file");
                }

                file << json.dump(4);
            } catch (const std::exception& exception) {
                std::fprintf(stderr, "failed to save: %s\n", exception.what());
            }
        }
    }
} // namespace imagegraph
