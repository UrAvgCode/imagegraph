#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba16f) uniform readonly image2D u_input;
layout (binding = 1, rgba16f) uniform writeonly image2D u_output;
layout (binding = 2) uniform sampler2D u_depth;

layout (location = 0) uniform float u_focus;
layout (location = 1) uniform float u_radius;

void main() {
    ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_input);

    if (texel.x >= size.x || texel.y >= size.y) {
        return;
    }

    vec2 uv = (vec2(texel) + 0.5) / vec2(size);
    float depth = texture(u_depth, uv).r;

    float blur_factor = abs(depth - u_focus);
    int radius = int(blur_factor * u_radius);

    vec4 color = vec4(0.0);
    float weight_sum = 0.0;

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {

            ivec2 offset = ivec2(x, y);
            float dist = length(vec2(x, y));

            if (dist > float(radius)) continue;

            ivec2 coord = clamp(texel + ivec2(x, y), ivec2(0), size - 1);
            vec4 sample_color = imageLoad(u_input, coord);

            float weight = 1.0 - (dist / float(radius + 1));

            color += sample_color * weight;
            weight_sum += weight;
        }
    }

    imageStore(u_output, texel, color / weight_sum);
}