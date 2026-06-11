#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba16f) uniform readonly image2D u_input;
layout (binding = 1, rgba16f) uniform writeonly image2D u_output;

layout (location = 0) uniform float u_saturation;
layout (location = 1) uniform float u_vibrance;

float luminance(vec3 rgb) {
    return dot(rgb, vec3(0.2126, 0.7152, 0.0722));
}

vec3 apply_saturation(vec3 rgb, float saturation) {
    float luma = luminance(rgb);
    return mix(vec3(luma), rgb, saturation);
}

vec3 apply_vibrance(vec3 rgb, float vibrance) {
    float luma = luminance(rgb);

    vec3 chroma_vector = rgb - vec3(luma);
    float chroma = length(chroma_vector);

    float vibrance_mask = 1.0 - chroma;
    float vibrance_gain = 1.0 + (vibrance * vibrance_mask);

    return vec3(luma) + chroma_vector * vibrance_gain;
}

void main() {
    ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_input);

    if (texel.x >= size.x || texel.y >= size.y) {
        return;
    }

    vec4 color = imageLoad(u_input, texel);
    color.rgb = apply_saturation(color.rgb, u_saturation);
    color.rgb = apply_vibrance(color.rgb, u_vibrance);
    imageStore(u_output, texel, color);
}
