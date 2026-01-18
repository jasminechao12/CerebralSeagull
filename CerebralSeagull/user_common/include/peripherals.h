#include <stdbool.h>
#include <stdint.h>


// HID and TUD
void user_tud_task();

uint32_t user_board_millis();

bool user_tud_hid_ready();

bool user_tud_hid_report(uint8_t report_id, void const* report, uint16_t len);



// Blinks
void user_blink_init();

float user_blink_predict(float sample);



// Servo
int servo_enable(uint8_t channel, uint8_t enabled);

int servo_set(uint8_t channel, uint8_t angle);
