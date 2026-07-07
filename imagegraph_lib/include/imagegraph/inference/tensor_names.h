#pragma once

#include <onnxruntime_cxx_api.h>

#include <string>
#include <vector>

namespace imagegraph::inference {
    class TensorNames {
    public:
        explicit TensorNames(std::vector<std::string>);

        TensorNames(const TensorNames&) = delete;
        TensorNames& operator=(const TensorNames&) = delete;

        const char* const* data() const;
        std::size_t size() const;

    private:
        std::vector<std::string> _names;
        std::vector<const char*> _c_strs;
    };

    TensorNames input_names(const Ort::Session&);
    TensorNames output_names(const Ort::Session&);
} // namespace imagegraph::inference
