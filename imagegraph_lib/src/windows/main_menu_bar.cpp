#include <windows/main_menu_bar.h>

#include <GLFW/glfw3.h>

#include <platform/file_dialog.h>

#include <cstdio>
#include <fstream>

namespace imagegraph {
    MainMenuBar::MainMenuBar(NodeEditor* node_editor) : _node_editor(node_editor) {}

    void MainMenuBar::draw() const {
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
            ImGui::EndMainMenuBar();
        }
    }

    void MainMenuBar::handle_shortcuts() const {
        if (ImGui::GetIO().WantTextInput) {
            return;
        }

        if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_O)) {
            open();
        }
        if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_S)) {
            save();
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
