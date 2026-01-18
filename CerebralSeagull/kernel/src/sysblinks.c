#include <unistd.h>
#include "blink_inference.h"

void blink_init() {
    k_blink_init();
}

float blink_predict(float sample) {
    return k_blink_parse_sample(sample);
}