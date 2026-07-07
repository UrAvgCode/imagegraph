#pragma once

#include <onnxruntime_cxx_api.h>

namespace imagegraph::inference {
    enum class Device { Cpu, Cuda };

    Ort::Session create_session(const char*, Device);
} // namespace imagegraph::inference
