#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, r16f) uniform readonly image2D u_input;
layout (binding = 1, r16f) uniform writeonly image2D u_output;

layout (location = 0) uniform float u_lower_threshold;
layout (location = 1) uniform float u_upper_threshold;

void main() {
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    ivec2 input_size = imageSize(u_input);
    if (any(greaterThanEqual(coordinate, input_size))) {
        return;
    }

    float value = imageLoad(u_input, coordinate).r;
    float result = float(value >= u_lower_threshold && value <= u_upper_threshold);
    imageStore(u_output, coordinate, vec4(result));
}
