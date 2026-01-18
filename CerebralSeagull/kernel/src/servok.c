#include <unistd.h>
#include <gpio.h>
#include <timer.h>
#include <servok.h>

#define UNUSED __attribute__((unused))

int sys_servo_enable(uint8_t channel, uint8_t enabled){
  (void) channel;
  (void) enabled;

  uint32_t period = 10;
  uint32_t prescalar = 480;

  timer_init(3, prescalar, period);

  gpio_init(GPIO_A, 10, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
  return 0;
}



int ticks_high1;
int ticks_low1;

// 20 ms period
// .6 ms = 0 deg
// 1.5 ms = 90 deg
// 2.4 ms = 180 deg
int sys_servo_set(uint8_t channel, uint8_t angle){
  if (channel == 1) {
    ticks_high1 = (6 + (angle/10));
    ticks_low1 = 200 - ticks_high1;
  }

  return angle;
}