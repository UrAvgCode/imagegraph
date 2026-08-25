#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba16f) uniform readonly image2D u_input;
layout (binding = 1, rgba16f) uniform writeonly image2D u_output;
layout (binding = 2) uniform sampler2D u_depth;

layout (location = 0) uniform float u_focus;
layout (location = 1) uniform float u_radius;

void main() {
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    ivec2 input_size = imageSize(u_input);
    if (any(greaterThanEqual(coordinate, input_size))) {
        return;
    }

    vec2 uv = (vec2(coordinate) + 0.5) / vec2(input_size);
    float depth = texture(u_depth, uv).r;

    float blur_factor = abs(depth - u_focus);
    int radius = int(blur_factor * u_radius);

    vec4 color = vec4(0.0);
    float weight_sum = 0.0;

    int radius_squared = radius * radius;
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            int distance_squared = x * x + y * y;

            if (distance_squared > radius_squared) {
                continue;
            }

            ivec2 sample_coordinate = clamp(coordinate + ivec2(x, y), ivec2(0), input_size - 1);
            float sample_distance = sqrt(float(distance_squared));
            float weight = 1.0 - (sample_distance / float(radius + 1));

            color += imageLoad(u_input, sample_coordinate) * weight;
            weight_sum += weight;
        }
    }

    imageStore(u_output, coordinate, color / weight_sum);
}
