#include <stdint.h>
#include <stdbool.h>
#include <usb_driver.h>

#include "tusb.h"

void sys_tud_task() {
    tud_task();
}

uint32_t sys_board_millis() {
    return board_millis();
}

bool sys_tud_hid_ready() {
    return tud_hid_ready();
}

bool sys_tud_hid_report(uint8_t report_id, void const* report, uint16_t len) {
    return tud_hid_report(report_id, report, len);
}

// USB CDC read - returns 0 on success, -1 on no data
int sys_cdc_read_byte(char *c) {
    if (tud_cdc_available() > 0) {
        uint32_t count = tud_cdc_read(c, 1);
        return (count == 1) ? 0 : -1;
    }
    return -1;
}