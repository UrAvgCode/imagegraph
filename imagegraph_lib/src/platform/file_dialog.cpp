#include <platform/file_dialog.h>

#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <nfd.hpp>
#include <nfd_glfw3.h>

#include <array>
#include <cstdlib>
#include <utility>

namespace {
    constexpr auto image_filters = std::array{nfdfilteritem_t{"Images", "jpg,jpeg,png,bmp,tga,psd,gif,hdr,pic,ppm,pgm"},
                                              nfdfilteritem_t{"PNG", "png"}, nfdfilteritem_t{"JPEG", "jpg,jpeg"},
                                              nfdfilteritem_t{"HDR", "hdr"}};

    constexpr auto json_filters = std::array{nfdfilteritem_t{"JSON", "json"}};

    const char* home_dir = std::getenv("HOME");
    std::filesystem::path default_path = home_dir ? home_dir : "";

    void set_default_directory(const char* path) {
        auto folder_path = std::filesystem::path(path).parent_path();
        if (!folder_path.empty()) {
            default_path = std::move(folder_path);
        }
    }

    template<std::size_t N>
    std::filesystem::path open_dialog(const std::array<nfdfilteritem_t, N>& filters) {
        auto parent_window = nfdwindowhandle_t();
        NFD_GetNativeWindowFromGLFWWindow(glfwGetCurrentContext(), &parent_window);

        auto path = NFD::UniquePath();
        const auto result = NFD::OpenDialog(path, filters.data(), filters.size(), default_path.c_str(), parent_window);

        if (result == NFD_OKAY) {
            set_default_directory(path.get());
            return path.get();
        }
        return "";
    }

    template<std::size_t N>
    std::filesystem::path save_dialog(const std::array<nfdfilteritem_t, N>& filters, const char* default_name) {
        auto parent_window = nfdwindowhandle_t();
        NFD_GetNativeWindowFromGLFWWindow(glfwGetCurrentContext(), &parent_window);

        auto path = NFD::UniquePath();
        const auto result = NFD::SaveDialog(path, filters.data(), filters.size(), default_path.c_str(), default_name,
                                            parent_window);

        if (result == NFD_OKAY) {
            set_default_directory(path.get());
            return path.get();
        }
        return "";
    }
} // namespace

namespace imagegraph::platform {
    std::filesystem::path open_image_dialog() { return open_dialog(image_filters); }

    std::filesystem::path save_image_dialog() { return save_dialog(image_filters, "untitled.jpg"); }

    std::filesystem::path open_json_dialog() { return open_dialog(json_filters); }

    std::filesystem::path save_json_dialog() { return save_dialog(json_filters, "untitled.json"); }
} // namespace imagegraph::platform
