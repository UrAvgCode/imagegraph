#pragma once

#include <image/image.h>

#include <filesystem>
#include <future>

namespace imagegraph::image {
    Image load_from_file(const std::filesystem::path&);

    std::future<Image> load_from_file_async(std::filesystem::path);

    void save_to_file(const Image&, const std::filesystem::path&);

    void save_to_file_async(Image, std::filesystem::path);
} // namespace imagegraph::image
