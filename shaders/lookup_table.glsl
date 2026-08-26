#version 460 core

layout (local_size_x = 256) in;

layout (std430, binding = 0) readonly buffer HistogramBuffer {
    uint histogram[256];
};

layout (std430, binding = 1) writeonly buffer LookupTableBuffer {
    float lut[256];
};

shared uint cdf[256];
shared uint cdf_min = 0xffffffffu;

void main() {
    uint bin_index = gl_LocalInvocationID.x;

    uint sum = 0u;
    for (uint i = 0u; i <= bin_index; ++i) {
        sum += histogram[i];
    }

    barrier();
    cdf[bin_index] = sum;
    barrier();

    if (histogram[bin_index] > 0u) {
        atomicMin(cdf_min, cdf[bin_index]);
    }
    barrier();

    uint total_pixels = cdf[255];

    if (total_pixels <= cdf_min) {
        lut[bin_index] = float(bin_index) / 255.0;}
    else if (cdf[bin_index] <= cdf_min) {
        lut[bin_index] = 0.0;
    } else {
        lut[bin_index] = float(cdf[bin_index] - cdf_min) / float(total_pixels - cdf_min);
    }
}
