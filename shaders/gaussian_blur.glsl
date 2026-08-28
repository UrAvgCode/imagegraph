#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba16f) uniform readonly image2D u_input;
layout (binding = 1, rgba16f) uniform writeonly image2D u_output;

layout (location = 0) uniform ivec2 u_blur_size;
layout (location = 1) uniform bool u_vertical_pass;

float gaussian(float x, float sigma) {
    return exp(-(x * x) / (2 * sigma * sigma));
}

void main() {
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    ivec2 input_size = imageSize(u_input);
    if (any(greaterThanEqual(coordinate, input_size))) {
        return;
    }

    int radius = u_vertical_pass ? u_blur_size.y : u_blur_size.x;
    if (radius == 0) {
        imageStore(u_output, coordinate, imageLoad(u_input, coordinate));
        return;
    }

    const float sigma = float(radius) / 3.0;

    vec4 color = vec4(0.0);
    float weight_sum = 0.0;

    for (int i = -radius; i <= radius; i++) {
        ivec2 offset = u_vertical_pass ? ivec2(0, i) : ivec2(i, 0);

        ivec2 sample_coordinate = clamp(coordinate + offset, ivec2(0), input_size - 1);
        vec4 sample_color = imageLoad(u_input, sample_coordinate);

        float weight = gaussian(float(i), sigma);
        color += sample_color * weight;
        weight_sum += weight;
    }

    imageStore(u_output, coordinate, color / weight_sum);
}
