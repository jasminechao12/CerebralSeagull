#ifndef BLINK_PROCESSOR_HPP
#define BLINK_PROCESSOR_HPP

#include <stdint.h>
#include <string.h>

extern "C" {
    float fabsf(float x);
    float fmaxf(float x, float y);
    float sqrtf(float x);
}

class BlinkProcessor {
private:
    static const int WINDOW_SIZE = 125;
    float buffer[WINDOW_SIZE];
    float v1, v2;
    // 5Hz Low-pass Butterworth coefficients at 250Hz sampling
    const float b[3] = {0.0036f, 0.0072f, 0.0036f};
    const float a[3] = {1.0000f, -1.8227f, 0.8372f};

public:
    BlinkProcessor() : v1(0), v2(0) {
        memset(buffer, 0, sizeof(buffer));
    }

    void add_sample(float x) {
        float out = b[0]*x + v1;
        v1 = b[1]*x - a[1]*out + v2;
        v2 = b[2]*x - a[2]*out;
        memmove(buffer, &buffer[1], (WINDOW_SIZE - 1) * sizeof(float));
        buffer[WINDOW_SIZE - 1] = out;
    }

    void get_features(float* f) {
        float max_v = -1e6f, min_v = 1e6f, sum_sq = 0, max_d = 0;
        for (int i = 0; i < WINDOW_SIZE; i++) {
            float v = buffer[i];
            if (v > max_v) max_v = v;
            if (v < min_v) min_v = v;
            if (i > 0) {
                float d = fabsf(v - buffer[i-1]);
                if (d > max_d) max_d = d;
            }
            sum_sq += v * v;
        }
        f[0] = max_v - min_v;         // PTP
        f[1] = fmaxf(fabsf(max_v), fabsf(min_v)); // MaxAmp
        f[2] = sum_sq / WINDOW_SIZE;  // Power
        f[3] = sqrtf(f[2]);           // RMS
        f[4] = max_d;                 // Max Derivative
    }
};

#endif