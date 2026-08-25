#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, r16f) uniform readonly image2D u_input;
layout (binding = 1, r16f) uniform writeonly image2D u_output;

void main() {
    ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_output);

    if (texel.x >= size.x || texel.y >= size.y) {
        return;
    }

    float value = imageLoad(u_input, texel).r;
    imageStore(u_output, texel, vec4(1.0 - value));
}
