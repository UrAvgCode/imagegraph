#version 460

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba16f) uniform readonly image2D u_input;
layout (binding = 1, rgba16f) uniform writeonly image2D u_output;

layout (location = 0) uniform float u_temperature;
layout (location = 1) uniform float u_tint;

void main() {
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    ivec2 input_size = imageSize(u_input);
    if (any(greaterThanEqual(coordinate, input_size))) {
        return;
    }

    vec4 color = imageLoad(u_input, coordinate);

    const float temperature_strength = 0.5;
    const float tint_strength = 0.5;

    vec3 gains = vec3(
    exp2(u_temperature * temperature_strength),
    exp2(-u_tint * tint_strength),
    exp2(-u_temperature * temperature_strength)
    );

    float magenta_gain = exp2(u_tint * tint_strength * 0.5);
    gains.r *= magenta_gain;
    gains.b *= magenta_gain;

    color.rgb *= gains;

    imageStore(u_output, coordinate, color);
}
