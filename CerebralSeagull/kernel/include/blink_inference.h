#ifndef BLINK_INFERENCE_H
#define BLINK_INFERENCE_H

#ifdef __cplusplus
extern "C" {
#endif

// Service Bridge Functions
void k_blink_init(void);
float k_blink_parse_sample(float raw_sample);

#ifdef __cplusplus
}
#endif

#endif