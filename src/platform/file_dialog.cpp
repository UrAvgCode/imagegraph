#include <platform/file_dialog.h>

#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <nfd.hpp>
#include <nfd_glfw3.h>

#include <array>

namespace {
    constexpr auto image_filters = std::array{nfdfilteritem_t{"Images", "jpg,jpeg,png,bmp,tga,psd,gif,hdr,pic,ppm,pgm"},
                                              nfdfilteritem_t{"PNG", "png"}, nfdfilteritem_t{"JPEG", "jpg,jpeg"},
                                              nfdfilteritem_t{"HDR", "hdr"}};
} // namespace

namespace imagegraph::platform {
    std::string open_image_dialog() {
        auto parent_window = nfdwindowhandle_t();
        NFD_GetNativeWindowFromGLFWWindow(glfwGetCurrentContext(), &parent_window);

        auto path = NFD::UniquePath();
        const auto result = NFD::OpenDialog(path, image_filters.data(), image_filters.size(), nullptr, parent_window);

        if (result == NFD_OKAY) {
            return path.get();
        }
        return "";
    }

    std::string save_image_dialog() {
        auto parent_window = nfdwindowhandle_t();
        NFD_GetNativeWindowFromGLFWWindow(glfwGetCurrentContext(), &parent_window);

        auto path = NFD::UniquePath();
        const auto result =
                NFD::SaveDialog(path, image_filters.data(), image_filters.size(), nullptr, nullptr, parent_window);

        if (result == NFD_OKAY) {
            return path.get();
        }
        return "";
    }
} // namespace imagegraph::platform
