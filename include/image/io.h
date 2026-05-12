#pragma once

#include <image/image.h>

#include <future>
#include <string>

namespace imagegraph::image {
    Image load_from_file(const char*);

    std::future<Image> load_from_file_async(std::string);

    void save_to_file(const Image&, const char*);

    void save_to_file_async(Image, std::string);
} // namespace imagegraph::image
