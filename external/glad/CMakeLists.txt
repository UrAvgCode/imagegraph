find_package(OpenGL REQUIRED)

add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include)
target_compile_features(glad PUBLIC c_std_99)
target_link_libraries(glad PUBLIC OpenGL::GL)
