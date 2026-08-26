#ifndef RADIUS
    #define RADIUS 2
#endif

#define LOCAL_SIZE_X 16
#define LOCAL_SIZE_Y 16

layout (local_size_x = LOCAL_SIZE_X, local_size_y = LOCAL_SIZE_Y) in;

layout (binding = 0, rgba16f) uniform readonly image2D u_input;
layout (binding = 1, rgba16f) uniform writeonly image2D u_output;

const int KERNEL_SIZE = RADIUS * 2 + 1;
const int SAMPLE_COUNT = KERNEL_SIZE * KERNEL_SIZE;
const int MEDIAN_INDEX = SAMPLE_COUNT / 2;

const int TILE_SIZE_X = LOCAL_SIZE_X + RADIUS * 2;
const int TILE_SIZE_Y = LOCAL_SIZE_Y + RADIUS * 2;
const int TILE_SAMPLE_COUNT = TILE_SIZE_X * TILE_SIZE_Y;

shared vec4 shared_tile[TILE_SAMPLE_COUNT];

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
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    ivec2 input_size = imageSize(u_input);

    #if RADIUS == 0
    if (any(greaterThanEqual(coordinate, input_size))) {
        return;
    }
    imageStore(u_output, coordinate, imageLoad(u_input, coordinate));
    return;
    #endif

    #if RADIUS > 0
    const ivec2 workgroup_origin = ivec2(gl_WorkGroupID.xy) * ivec2(LOCAL_SIZE_X, LOCAL_SIZE_Y);
    const int local_index = int(gl_LocalInvocationID.y) * LOCAL_SIZE_X + int(gl_LocalInvocationID.x);
    const int local_count = LOCAL_SIZE_X * LOCAL_SIZE_Y;

    for (int tile_index = local_index; tile_index < TILE_SAMPLE_COUNT; tile_index += local_count) {
        const int tile_x = tile_index % TILE_SIZE_X;
        const int tile_y = tile_index / TILE_SIZE_X;

        const ivec2 source_position = clamp(
            workgroup_origin +
            ivec2(tile_x - RADIUS, tile_y - RADIUS),
            ivec2(0),
            input_size - ivec2(1)
        );

        shared_tile[tile_index] = imageLoad(u_input, source_position);
    }

    barrier();

    if (any(greaterThanEqual(coordinate, input_size))) {
        return;
    }

    const int tile_origin_x = int(gl_LocalInvocationID.x) + RADIUS;
    const int tile_origin_y = int(gl_LocalInvocationID.y) + RADIUS;

    float red_values[SAMPLE_COUNT];
    float green_values[SAMPLE_COUNT];
    float blue_values[SAMPLE_COUNT];

    int sample_index = 0;

    for (int y = -RADIUS; y <= RADIUS; ++y) {
        for (int x = -RADIUS; x <= RADIUS; ++x) {
            const int tile_x = tile_origin_x + x;
            const int tile_y = tile_origin_y + y;

            const vec4 sample_value = shared_tile[tile_y * TILE_SIZE_X + tile_x];

            red_values[sample_index] = sample_value.r;
            green_values[sample_index] = sample_value.g;
            blue_values[sample_index] = sample_value.b;

            ++sample_index;
        }
    }

    const int center_index = tile_origin_y * TILE_SIZE_X + tile_origin_x;
    const float alpha = shared_tile[center_index].a;

    const vec4 result = vec4(
    quickselect(red_values),
    quickselect(green_values),
    quickselect(blue_values),
    alpha
    );

    imageStore(u_output, coordinate, result);
    #endif
}
