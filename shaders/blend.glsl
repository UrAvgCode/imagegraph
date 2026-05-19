#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba32f) uniform readonly image2D u_base;
layout (binding = 1, rgba32f) uniform readonly image2D u_blend;
layout (binding = 2) uniform sampler2D u_mask;
layout (binding = 3, rgba32f) uniform writeonly image2D u_output;

uniform bool u_use_mask;

void main() {
    ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_base);

    if (texel.x >= size.x || texel.y >= size.y) {
        return;
    }

    vec4 base = imageLoad(u_base, texel);
    vec4 blend = imageLoad(u_blend, texel);

    vec2 uv = (vec2(texel) + 0.5) / vec2(size);
    float mask = u_use_mask ? texture(u_mask, uv).r : 0.5;

    vec4 color = mix(base, blend, mask);
    imageStore(u_output, texel, color);
}
