#version 460 core

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba16f) uniform readonly image2D u_input;
layout (binding = 1, rgba16f) uniform writeonly image2D u_output;

layout (std430, binding = 1) readonly buffer LookupTableBuffer {
    float lut[256];
};

float luminance(vec3 rgb) {
    return dot(rgb, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    ivec2 input_size = imageSize(u_input);
    if (any(greaterThanEqual(coordinate, input_size))) {
        return;
    }

    vec4 color = imageLoad(u_input, coordinate);
    float color_luminance = clamp(luminance(color.rgb), 0.0, 1.0);

    uint bin = uint(round(color_luminance * 255.0));
    float new_luminance = lut[bin];

    vec3 rgb;
    if (color_luminance > 0) {
        rgb = color.rgb * (new_luminance / color_luminance);
    } else {
        rgb = vec3(new_luminance);
    }

    imageStore(u_output, coordinate, vec4(rgb, color.a));
}
