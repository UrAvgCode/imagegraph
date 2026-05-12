#include <image/io.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <thread>
#include <utility>
#include <vector>

namespace imagegraph::image {
    Image load_from_file(const char* path) {
        int width = 0;
        int height = 0;
        int channels = 0;

        stbi_ldr_to_hdr_scale(1.0f);
        stbi_ldr_to_hdr_gamma(1.0f);
        const auto data = stbi_loadf(path, &width, &height, &channels, STBI_rgb_alpha);
        if (!data) {
            std::fprintf(stderr, "failed to load image: %s (%s)\n", path, stbi_failure_reason());
            return {};
        }

        const auto result = Image(width, height, STBI_rgb_alpha, data);
        stbi_image_free(data);

        return result;
    }

    std::future<Image> load_from_file_async(std::string path) {
        return std::async(std::launch::async, [path = std::move(path)] { return load_from_file(path.c_str()); });
    }

    void save_to_file(const Image& image, const char* path) {
        const auto path_view = std::string_view(path);

        if (path_view.ends_with(".hdr")) {
            stbi_write_hdr(path, image.width(), image.height(), image.channels(), image.data());
            return;
        }

        auto data = std::vector<std::uint8_t>(image.size());
        for (std::size_t i = 0; i < image.size(); ++i) {
            const float value = std::clamp(image.data()[i], 0.0f, 1.0f);
            data[i] = static_cast<std::uint8_t>(std::lround(value * 255.0f));
        }

        if (path_view.ends_with(".jpg") || path_view.ends_with(".jpeg")) {
            constexpr int quality = 95;
            stbi_write_jpg(path, image.width(), image.height(), image.channels(), data.data(), quality);
            return;
        }

        if (path_view.ends_with(".bmp")) {
            stbi_write_bmp(path, image.width(), image.height(), image.channels(), data.data());
            return;
        }

        if (path_view.ends_with(".tga")) {
            stbi_write_tga(path, image.width(), image.height(), image.channels(), data.data());
            return;
        }

        const auto stride = static_cast<int>(image.width() * image.channels() * sizeof(std::uint8_t));
        stbi_write_png(path, image.width(), image.height(), image.channels(), data.data(), stride);
    }

    void save_to_file_async(Image image, std::string path) {
        std::thread([image = std::move(image), path = std::move(path)] { save_to_file(image, path.c_str()); }).detach();
    }
} // namespace imagegraph::image
