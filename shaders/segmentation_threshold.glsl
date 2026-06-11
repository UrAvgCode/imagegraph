#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0) uniform sampler2D u_input;
layout (binding = 1, r16f) uniform writeonly image2D u_output;

layout (location = 0) uniform float u_threshold;

void main() {
    ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_output);

    if (texel.x >= size.x || texel.y >= size.y) {
        return;
    }

    vec2 uv = (vec2(texel) + 0.5) / vec2(size);
    float value = step(u_threshold, texture(u_input, uv).r);
    vec4 color = vec4(value, value, value, 1.0);
    imageStore(u_output, texel, color);
}
