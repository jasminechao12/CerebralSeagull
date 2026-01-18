/**
 * @file 
 *
 * @brief      
 *
 * @date       
 *
 * @author     
 */

#include <stdint.h>
#include <debug.h>
#include <syscall.h>
#include <syscall_thread.h>
#include <sysusb.h>
#include <sysblink.h>
#include <uart.h>
#include <servok.h>
#include <printk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED __attribute__((unused))

// Exception Stack Frame
struct exception_stack_frame {
  volatile uint32_t R0;     /**<  Caller Saved Register 0 */
  volatile uint32_t R1;     /**<  Caller Saved Register 1 */
  volatile uint32_t R2;     /**<  Caller Saved Register 2 */
  volatile uint32_t R3;     /**<  Caller Saved Register 3 */
  volatile uint32_t R12;    /**<  Intra-procedure-call scratch register */
  volatile uint32_t LR;     /**<  Link Register */
  volatile uint32_t PC;     /**<  Program Counter */
  volatile uint32_t PSR;    /**<  Program Status Register */
  volatile uint32_t arg5;
};


void svc_c_handler(struct exception_stack_frame* frame) {
  int svc_number = (uint8_t)(*((uint16_t *)(frame->PC - 2)) & 0xFF);

  switch ( svc_number ) {
    case 0:
      frame->R0 = (uint32_t) sys_sbrk((int)frame->R0);
      break;

    case 1:
      frame->R0 = (uint32_t) sys_write((int)frame->R0, (char *)frame->R1, (int)frame->R2);
      break;
    
    case 2:
      break;
    
    case 3:
      break;

    case 4:
      break;

    case 5:
      break;

    case 6:
      frame->R0 = (uint32_t) sys_read((int)frame->R0, (char *)frame->R1, (int)frame->R2);
      break;

    case 7:
      sys_exit((int)frame->R0);
      break;

    case 8:
      break;
      
    case 9:
      frame->R0 = sys_thread_init((uint32_t)frame->R0, (uint32_t)frame->R1, (void *)frame->R2, (uint32_t)frame->R3);
      break;
      
    case 10:
      frame->R0 = sys_thread_create((void *)frame->R0, (uint32_t)frame->R1, (uint32_t)frame->R2, (uint32_t)frame->R3, (void *)frame->arg5);
      break;
      
    case 11:
      sys_thread_kill();
      break;
      
    case 12:
      frame->R0 = sys_scheduler_start((uint32_t)frame->R0);
      break;
      
    case 13:
      frame->R0 = (uint32_t)sys_mutex_init((uint32_t)frame->R0);
      break;
      
    case 14:
      sys_mutex_lock((kmutex_t *)frame->R0);
      break;
      
    case 15:
      sys_mutex_unlock((kmutex_t *)frame->R0);
      break;
      
    case 16:
      sys_wait_until_next_period();
      break;
      
    case 17:
      frame->R0 = sys_get_time();
      break;
      
    case 18:
      break;
      
    case 19:
      frame->R0 = sys_get_priority();
      break;
      
    case 20:
      frame->R0 = sys_thread_time();
      break;
      
    case 21:
      sys_tud_task();
      break;

    case 22:
      frame->R0 = sys_board_millis();
      break;

    case 23:
      frame->R0 = sys_tud_hid_ready();
      break;

    case 24:
      frame->R0 = sys_tud_hid_report((uint8_t)frame->R0, (void const*)frame->R1, (uint16_t)frame->R2);
      break;

    case 25:
      blink_init();
      break;
    
    case 26:
      frame->R0 = blink_predict((float)frame->R0);
      break;

    case 27:
      frame->R0 = uart_get_byte((char *)frame->R0);
      break;

    case 28:
      frame->R0 = (uint32_t) sys_servo_enable((uint8_t)frame->R0, (uint8_t)frame->R1);
      break;
      
    case 29:
      frame->R0 = (uint32_t) sys_servo_set((uint8_t)frame->R0, (uint8_t)frame->R1);
      break;

    default:
      DEBUG_PRINT( "Not implemented, svc num %d\n", svc_number );
      ASSERT( 0 );
  }
}
