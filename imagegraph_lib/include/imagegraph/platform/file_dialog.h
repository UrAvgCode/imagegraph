#pragma once

#include <filesystem>

namespace imagegraph::platform {
    std::filesystem::path open_image_dialog();
    std::filesystem::path save_image_dialog();

    std::filesystem::path open_json_dialog();
    std::filesystem::path save_json_dialog();
} // namespace imagegraph::platform
