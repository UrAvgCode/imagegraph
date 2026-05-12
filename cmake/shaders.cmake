file(GLOB_RECURSE shader_files CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/shaders/*.glsl")

set(output_root "${CMAKE_BINARY_DIR}/generated")
set(output_dir "${output_root}/shader")
file(MAKE_DIRECTORY "${output_dir}")

set(generated_headers)
foreach (shader_file IN LISTS shader_files)
    get_filename_component(shader_name "${shader_file}" NAME_WE)
    set(output_file "${output_dir}/${shader_name}.h")

    add_custom_command(
            OUTPUT ${output_file}
            COMMAND ${CMAKE_COMMAND}
            -DINPUT=${shader_file}
            -DOUTPUT=${output_file}
            -DNAME=${shader_name}
            -P ${CMAKE_SOURCE_DIR}/cmake/embed_shader.cmake
            DEPENDS ${shader_file}
            VERBATIM
    )

    list(APPEND generated_headers "${output_file}")
endforeach ()

add_custom_target(generate_shader_headers DEPENDS ${generated_headers})

add_library(shader_headers INTERFACE)
add_dependencies(shader_headers generate_shader_headers)
target_include_directories(shader_headers INTERFACE ${output_root})
