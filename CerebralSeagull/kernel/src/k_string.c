#include <stdint.h>

float fabsf(float x) {
    return (x < 0) ? -x : x;
}

float fmaxf(float x, float y) {
    return (x > y) ? x : y;
}

/**
 * Fast Inverse Square Root (Quake 3 algorithm)
 * A lightweight way to get sqrt in a kernel without libm.
 */
float sqrtf(float number) {
    if (number <= 0) return 0;
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) ); // 1st iteration
    
    return 1.0f / y;
}