#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba16f) uniform readonly image2D u_input;
layout (binding = 1, r16f) uniform writeonly image2D u_output;

layout (location = 0) uniform vec3 u_key_color;
layout (location = 1) uniform float u_tolerance;

void main() {
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    ivec2 input_size = imageSize(u_input);
    if (any(greaterThanEqual(coordinate, input_size))) {
        return;
    }

    vec3 color = imageLoad(u_input, coordinate).rgb;
    float distance_to_key = distance(color, u_key_color);
    float mask = distance_to_key <= u_tolerance ? 1.0 : 0.0;

    imageStore(u_output, coordinate, vec4(mask));
}
