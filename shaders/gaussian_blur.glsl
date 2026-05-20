#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba32f) uniform readonly image2D u_input;
layout (binding = 1, rgba32f) uniform writeonly image2D u_output;

layout (location = 0) uniform ivec2 u_blur_size;
layout (location = 1) uniform bool u_vertical_pass;

float gaussian(float x, float sigma) {
    float s = sigma * sigma;
    return exp(-0.5 * (x * x) / s);
}

void main() {
    ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_input);

    if (texel.x >= size.x || texel.y >= size.y) {
        return;
    }

    int radius = u_vertical_pass ? u_blur_size.y : u_blur_size.x;
    float sigma = float(radius) / 3.0;

    vec4 color = vec4(0.0);
    float weight_sum = 0.0;

    for (int i = -radius; i <= radius; i++) {
        ivec2 offset = u_vertical_pass ? ivec2(0, i) : ivec2(i, 0);
        ivec2 coord = clamp(texel + offset, ivec2(0), size - 1);

        float weight = gaussian(float(i), sigma);
        vec4 sample_color = imageLoad(u_input, coord);

        color += sample_color * weight;
        weight_sum += weight;
    }

    imageStore(u_output, texel, color / weight_sum);
}
