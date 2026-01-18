#include <stdbool.h>
#include <stdint.h>

void sys_tud_task();

uint32_t sys_board_millis();

bool sys_tud_hid_ready();

bool sys_tud_hid_report(uint8_t report_id, void const* report, uint16_t len);

// USB CDC read - returns 0 on success, -1 on no data (same signature as uart_read)
int sys_cdc_read_byte(char *c);