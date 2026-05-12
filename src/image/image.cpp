#include <image/image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>

#include <cassert>

namespace imagegraph::image {
    Image::Image() : _width(0), _height(0), _channels(0) {}

    Image::Image(const int width, const int height, const int channels) :
        _width(width), _height(height), _channels(channels) {
        _data.resize(width * height * channels);
    }

    Image::Image(const int width, const int height, const int channels, const float* data) :
        Image(width, height, channels) {
        _data.assign(data, data + width * height * channels);
    }

    float& Image::operator[](const std::size_t i) {
        assert(i < _data.size());
        return _data[i];
    }

    const float& Image::operator[](const std::size_t i) const {
        assert(i < _data.size());
        return _data[i];
    }

    void Image::resize(const int width, const int height) {
        if (width == _width && height == _height)
            return;

        auto resized_data = std::vector<float>(width * height * _channels);
        stbir_resize_float_linear(_data.data(), _width, _height, 0, resized_data.data(), width, height, 0,
                                  static_cast<stbir_pixel_layout>(_channels));

        _width = width;
        _height = height;
        _data = std::move(resized_data);
    }

    int Image::width() const { return _width; }

    int Image::height() const { return _height; }

    int Image::channels() const { return _channels; }

    std::size_t Image::size() const { return _data.size(); }

    std::size_t Image::size_in_bytes() const { return _data.size() * sizeof(float); }

    float* Image::data() { return _data.data(); }

    const float* Image::data() const { return _data.data(); }
} // namespace imagegraph::image
