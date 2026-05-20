#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba32f) uniform readonly image2D u_input;
layout (binding = 1, rgba32f) uniform writeonly image2D u_output;

layout(location = 0) uniform float u_brightness;
layout(location = 1) uniform float u_contrast;

void main() {
    ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_input);

    if (texel.x >= size.x || texel.y >= size.y) {
        return;
    }

    vec4 color = imageLoad(u_input, texel);
    color.rgb = (color.rgb - 0.5) * u_contrast + 0.5 + u_brightness;
    imageStore(u_output, texel, color);
}