#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba16f) uniform readonly image2D u_base;
layout (binding = 1, rgba16f) uniform readonly image2D u_blend;
layout (binding = 2) uniform sampler2D u_mask;
layout (binding = 3, rgba16f) uniform writeonly image2D u_output;

layout (location = 0) uniform bool u_use_mask;

void main() {
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    ivec2 base_size = imageSize(u_base);
    if (any(greaterThanEqual(coordinate, base_size))) {
        return;
    }

    vec4 base = imageLoad(u_base, coordinate);
    vec4 blend = imageLoad(u_blend, coordinate);

    vec2 uv = (vec2(coordinate) + 0.5) / vec2(base_size);
    float mask = u_use_mask ? texture(u_mask, uv).r : 0.5;

    vec4 color = mix(base, blend, mask);
    imageStore(u_output, coordinate, color);
}
