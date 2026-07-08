#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <benchmark/benchmarks/blur_benchmark.h>
#include <benchmark/benchmarks/depth_model_benchmark.h>
#include <benchmark/benchmarks/grayscale_benchmark.h>
#include <benchmark/benchmarks/segment_model_benchmark.h>

#include <cstdio>

int main() {
    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const auto window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::fprintf(stderr, "failed to initialize glad\n");
        return 1;
    }

    {
        auto segment_benchmark = imagegraph::benchmark::SegmentModelBenchmark();
        segment_benchmark.run();
    }
    {
        auto depth_benchmark = imagegraph::benchmark::DepthModelBenchmark();
        depth_benchmark.run();
    }
    {
        auto grayscale_benchmark = imagegraph::benchmark::GrayscaleBenchmark();
        grayscale_benchmark.run();
    }
    {
        auto blur_benchmark = imagegraph::benchmark::BlurBenchmark();
        blur_benchmark.run();
    }
}
