#include <stdlib.h>
#include <string.h>

#include <gpio.h>
#include <usb_driver.h>
#include <syscall_thread.h>
#include <systick.h>
#include <timer.h>
#include <nvic.h>

#include "arm.h"
#include "kernel.h"
#include "timer.h"
#include "i2c.h"
#include "printk.h"
#include "uart.h"
#include "stdint.h"
#include "syscall.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "arm.h"
#include "tusb.h"


// BRR values for 115200 baud:
// 16 MHz APB1: 0x008B (139) = 16000000 / (16 * 115200)
// 42 MHz APB1: 0x016C (364) = 42000000 / (16 * 115200)  
// 48 MHz APB1: 0x01A1 (417) = 48000000 / (16 * 115200) = 26.0417
//    Mantissa = 26, Fraction = 1 (0.0417 * 16 ≈ 1)
// 84 MHz APB1: 0x002D (45)  = 84000000 / (16 * 115200)
// Try 48 MHz (matches CPU_FREQ):
#define BAUD_RATE_115200 0x01A1


// Tells systick that the scheduler isn't started
bool scheduler_started = false;


void cerebral_seagulls_init() {
    init_349();

    // UART
    uart_init(BAUD_RATE_115200);

    // USB
    usb_init();
}

int kernel_main() {
  cerebral_seagulls_init();

  enter_user_mode();

  while(1){
  }

  return 0;
}
