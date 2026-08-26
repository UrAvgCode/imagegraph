#version 460 core

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba16f) uniform readonly image2D u_input;

layout (std430, binding = 0) buffer HistogramBuffer {
    uint histogram[256];
};

shared uint local_histogram[256];

float luminance(vec3 rgb) {
    return dot(rgb, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
    uint local_index = gl_LocalInvocationIndex;
    local_histogram[local_index] = 0u;

    barrier();

    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    ivec2 input_size = imageSize(u_input);

    if (all(lessThan(coordinate, input_size))) {
        vec4 color = imageLoad(u_input, coordinate);
        float lum = clamp(luminance(color.rgb), 0.0, 1.0);
        uint bin = uint(round(lum * 255.0));

        atomicAdd(local_histogram[bin], 1u);
    }

    barrier();

    uint count = local_histogram[local_index];
    if (count > 0u) {
        atomicAdd(histogram[local_index], count);
    }
}
