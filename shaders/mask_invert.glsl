#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, r16f) uniform readonly image2D u_input;
layout (binding = 1, r16f) uniform writeonly image2D u_output;

void main() {
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    ivec2 input_size = imageSize(u_input);
    if (any(greaterThanEqual(coordinate, input_size))) {
        return;
    }

    float value = imageLoad(u_input, coordinate).r;
    imageStore(u_output, coordinate, vec4(1.0 - value));
}
