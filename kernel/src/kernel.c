#include <stdlib.h>
#include <string.h>

#include <gpio.h>

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


#define BAUD_RATE_115200 0x8B

int kernel_main() {
  init_349();

  uart_init(BAUD_RATE_115200);

  enter_user_mode();

  while(1){
  }

  return 0;
}
