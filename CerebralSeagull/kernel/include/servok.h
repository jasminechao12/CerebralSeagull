#ifndef _SERVOK_H_
#define _SERVOK_H_

extern int pwm_channel0;
extern int pwm_channel1;
extern int ch0_en;
extern int ch1_en;


extern int ticks_high1;
extern int ticks_low1;

extern int ticks_high2;
extern int ticks_low2;

int sys_servo_enable(uint8_t channel, uint8_t enabled);

int sys_servo_set(uint8_t channel, uint8_t angle);

#endif /* _SERVOK_H_ */