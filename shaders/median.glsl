#ifndef RADIUS
    #define RADIUS 2
#endif

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0, rgba16f) uniform readonly image2D u_input;
layout (binding = 1, rgba16f) uniform writeonly image2D u_output;

const int KERNEL_SIZE = RADIUS * 2 + 1;
const int SAMPLE_COUNT = KERNEL_SIZE * KERNEL_SIZE;
const int MEDIAN_INDEX = SAMPLE_COUNT / 2;

float quickselect(inout float values[SAMPLE_COUNT]) {
    int left = 0;
    int right = SAMPLE_COUNT - 1;

    for (int pass = 0; pass < SAMPLE_COUNT; ++pass) {
        if (left >= right) {
            break;
        }

        const int middle = left + (right - left) / 2;
        const float pivot = values[middle];

        int i = left;
        int j = right;

        while (i <= j) {
            while (i <= right && values[i] < pivot) {
                ++i;
            }

            while (j >= left && values[j] > pivot) {
                --j;
            }

            if (i <= j) {
                const float temporary = values[i];
                values[i] = values[j];
                values[j] = temporary;

                ++i;
                --j;
            }
        }

        if (MEDIAN_INDEX <= j) {
            right = j;
        } else if (MEDIAN_INDEX >= i) {
            left = i;
        } else {
            break;
        }
    }

    return values[MEDIAN_INDEX];
}

void main() {
    ivec2 texel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(u_input);

    if (texel.x >= size.x || texel.y >= size.y) {
        return;
    }

    #if RADIUS == 0
    imageStore(u_output, texel, imageLoad(u_input, texel));
    return;
    #endif

    #if RADIUS > 0
    float red_values[SAMPLE_COUNT];
    float green_values[SAMPLE_COUNT];
    float blue_values[SAMPLE_COUNT];

    int sample_index = 0;

    for (int y = -RADIUS; y <= RADIUS; ++y) {
        for (int x = -RADIUS; x <= RADIUS; ++x) {
            const ivec2 sample_position = clamp(texel + ivec2(x, y), ivec2(0), size - ivec2(1));
            const vec4 sample_value = imageLoad(u_input, sample_position);

            red_values[sample_index] = sample_value.r;
            green_values[sample_index] = sample_value.g;
            blue_values[sample_index] = sample_value.b;

            ++sample_index;
        }
    }

    const vec4 result = vec4(
    quickselect(red_values),
    quickselect(green_values),
    quickselect(blue_values),
    imageLoad(u_input, texel).a
    );

    imageStore(u_output, texel, result);
    #endif
}
