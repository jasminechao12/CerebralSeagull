#include "blink_processing.hpp"
#include "blink_inference.h"

static BlinkProcessor processor;

extern "C" {
    void k_blink_init() {
        // No heavy model to init anymore!
    }

    float k_blink_parse_sample(float raw_sample) {
        processor.add_sample(raw_sample);
        float f[5];
        processor.get_features(f);

        // HEURISTIC: A blink usually has a sharp "spike" (f[4]) 
        // and a significant energy burst (f[3]).
        // These thresholds (0.4 and 0.2) can be tuned.
        if (f[4] > 0.4f && f[3] > 0.2f) {
            return 1.0f; // High probability of blink
        }
        return 0.0f;
    }
}