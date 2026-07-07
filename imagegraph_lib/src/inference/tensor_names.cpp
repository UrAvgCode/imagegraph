#include <imagegraph/inference/tensor_names.h>

#include <utility>

namespace imagegraph::inference {
    TensorNames::TensorNames(std::vector<std::string> names) : _names(std::move(names)), _c_strs(_names.size()) {
        for (std::size_t i = 0; i < _names.size(); ++i) {
            _c_strs[i] = _names[i].c_str();
        }
    }

    const char* const* TensorNames::data() const { return _c_strs.data(); }

    std::size_t TensorNames::size() const { return _c_strs.size(); }

    TensorNames input_names(const Ort::Session& session) {
        const auto count = session.GetInputCount();
        auto names = std::vector<std::string>(count);

        const auto allocator = Ort::AllocatorWithDefaultOptions();
        for (std::size_t i = 0; i < count; ++i) {
            auto name = session.GetInputNameAllocated(i, allocator);
            names[i] = name.get();
        }

        return TensorNames(std::move(names));
    }

    TensorNames output_names(const Ort::Session& session) {
        const auto count = session.GetOutputCount();
        auto names = std::vector<std::string>(count);

        const auto allocator = Ort::AllocatorWithDefaultOptions();
        for (std::size_t i = 0; i < count; ++i) {
            auto name = session.GetOutputNameAllocated(i, allocator);
            names[i] = name.get();
        }

        return TensorNames(std::move(names));
    }
} // namespace imagegraph::inference
