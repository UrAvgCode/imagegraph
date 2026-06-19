#pragma once

#include <vector>

namespace imagegraph::image {
    class Image {
    public:
        Image();
        Image(int width, int height, int channels);
        Image(int width, int height, int channels, const float* data);

        float& operator[](std::size_t);
        const float& operator[](std::size_t) const;

        void resize(int width, int height);

        int width() const;
        int height() const;
        int channels() const;

        std::size_t size() const;
        std::size_t size_in_bytes() const;

        float* data();
        const float* data() const;

    private:
        int _width;
        int _height;
        int _channels;

        std::vector<float> _data;
    };
} // namespace imagegraph::image
