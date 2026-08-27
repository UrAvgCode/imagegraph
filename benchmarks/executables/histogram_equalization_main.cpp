#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <benchmark/benchmarks/histogram_equalization_benchmark.h>

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

    auto histogram_benchmark = imagegraph::benchmark::HistogramEqualizationBenchmark();
    histogram_benchmark.run();
}
