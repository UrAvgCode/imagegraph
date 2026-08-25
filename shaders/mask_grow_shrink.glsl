#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, r16f) uniform readonly image2D u_input;
layout (binding = 1, r16f) uniform writeonly image2D u_output;

layout (location = 0) uniform int u_amount;

void main() {
    ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_output);

    if (texel.x >= size.x || texel.y >= size.y) {
        return;
    }

    const int radius = abs(u_amount);
    if (radius == 0) {
        float value = imageLoad(u_input, texel).r;
        imageStore(u_output, texel, vec4(value));
        return;
    }

    const bool grow = u_amount > 0;
    float result = grow ? 0.0 : 1.0;

    const int radius_squared = radius * radius;
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            if (x * x + y * y > radius_squared) {
                continue;
            }

            ivec2 sample_pos = texel + ivec2(x, y);
            if (sample_pos.x < 0 || sample_pos.y < 0 || sample_pos.x >= size.x || sample_pos.y >= size.y) {
                continue;
            }

            float value = imageLoad(u_input, sample_pos).r;
            result = grow ? max(result, value) : min(result, value);
        }
    }

    imageStore(u_output, texel, vec4(result));
}
