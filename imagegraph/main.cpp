#include <glad/gl.h>

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <nfd.h>
#include <nfd_glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <imagegraph/graph/graph.h>
#include <imagegraph/inference/environment.h>
#include <imagegraph/windows/main_menu_bar.h>
#include <imagegraph/windows/node_editor.h>
#include <imagegraph/windows/output_view.h>

#include <cstdio>
#include <fstream>

static void glfw_error_callback(const int error, const char* description) {
    std::fprintf(stderr, "glfw error %d: %s\n", error, description);
}

static void initialize_glfw() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::fprintf(stderr, "failed to initialize glfw\n");
        std::exit(1);
    }
}

static void initialize_nfd() {
    if (NFD_Init() != NFD_OKAY) {
        std::fprintf(stderr, "failed to initialize nfd: %s\n", NFD_GetError());
        std::exit(1);
    }

    if (!NFD_SetDisplayPropertiesFromGLFW()) {
        std::fprintf(stderr, "set nfd display properties from glfw failed\n");
    }
}

static GLFWwindow* create_window() {
    const float scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    const int width = static_cast<int>(1280.0f * scale);
    const int height = static_cast<int>(800.0f * scale);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    const auto window = glfwCreateWindow(width, height, "Image Graph", nullptr, nullptr);
    if (!window) {
        std::fprintf(stderr, "failed to create window\n");
        std::exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    return window;
}

static void initialize_glad() {
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::fprintf(stderr, "failed to initialize glad\n");
        std::exit(1);
    }
}

static void initialize_imgui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    const float scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

    auto& style = ImGui::GetStyle();
    ImGui::StyleColorsDark(&style);
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;

    auto font_config = ImFontConfig();
    font_config.OversampleH = 8;
    font_config.OversampleV = 8;
    font_config.PixelSnapH = false;
    io.Fonts->AddFontDefaultVector(&font_config);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

static void setup_dockspace() {
    const auto dockspace_id = ImGui::GetID("DockSpace");
    ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    static bool dockspace_initialized = false;
    if (!dockspace_initialized) {
        dockspace_initialized = true;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        ImGuiID dock_left;
        ImGuiID dock_right;
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, &dock_left, &dock_right);

        ImGui::DockBuilderDockWindow("Output", dock_left);
        ImGui::DockBuilderDockWindow("Node Editor", dock_right);

        ImGui::DockBuilderFinish(dockspace_id);
    }
}

static void run(GLFWwindow* window) {
    auto graph = imagegraph::graph::Graph();
    auto node_editor = imagegraph::NodeEditor(&graph);
    const auto output_view = imagegraph::OutputView(&graph);
    const auto main_menu_bar = imagegraph::MainMenuBar(&node_editor);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        graph.evaluate();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        main_menu_bar.draw();
        setup_dockspace();

        ImGui::Begin("Node Editor");
        node_editor.draw();
        ImGui::End();

        ImGui::Begin("Output");
        output_view.draw();
        ImGui::End();

        ImGui::Render();
        int display_width;
        int display_height;
        glfwGetFramebufferSize(window, &display_width, &display_height);

        glViewport(0, 0, display_width, display_height);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

int main() {
    initialize_glfw();
    initialize_nfd();

    const auto window = create_window();

    initialize_glad();
    initialize_imgui(window);

    imagegraph::inference::init_environment();

    run(window);

    imagegraph::inference::destroy_environment();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    NFD_Quit();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
