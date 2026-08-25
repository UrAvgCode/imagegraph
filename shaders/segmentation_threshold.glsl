#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0) uniform sampler2D u_input;
layout (binding = 1, r16f) uniform writeonly image2D u_output;

layout (location = 0) uniform float u_threshold;

void main() {
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    ivec2 output_size = imageSize(u_output);
    if (any(greaterThanEqual(coordinate, output_size))) {
        return;
    }

    vec2 uv = (vec2(coordinate) + 0.5) / vec2(output_size);
    float value = step(u_threshold, texture(u_input, uv).r);
    imageStore(u_output, coordinate, vec4(value));
}
