/**
 * @file   swi_stubs.S
 *
 * @brief  Stub functions for newlib and 349libc syscalls
 *
 * @date
 * @author
 */

.cpu cortex-m4
.syntax unified
.section .swi_stub
.thumb

#include "../../kernel/include/svc_num.h"

.global _sbrk
_sbrk:
  SVC SVC_SBRK
  bx lr

.global _write
_write:
  SVC SVC_WRITE
  bx lr

.global _close
_close:
  SVC SVC_CLOSE
  bx lr

.global _fstat
_fstat:
  SVC SVC_FSTAT
  bx lr

.global _isatty
_isatty:
  SVC SVC_ISATTY
  bx lr

.global _lseek
_lseek:
  SVC SVC_LSEEK
  bx lr

.global _read
_read:
  SVC SVC_READ
  bx lr

.global _exit
_exit:
  SVC SVC_EXIT
  bx lr

.type thread_init, %function
.global thread_init
thread_init:
  SVC SVC_THR_INIT
  bx lr

.type thread_create, %function
.global thread_create
thread_create:
  SVC SVC_THR_CREATE
  bx lr

.type thread_kill, %function
.global thread_kill
thread_kill:
  SVC SVC_THR_KILL
  bx lr

.type scheduler_start, %function
.global scheduler_start
scheduler_start:
  SVC SVC_SCHD_START
  bx lr

.type mutex_init, %function
.global mutex_init
mutex_init:
  SVC SVC_MUT_INIT
  bx lr

.type mutex_lock, %function
.global mutex_lock
mutex_lock:
  SVC SVC_MUT_LOK
  bx lr

.type mutex_unlock, %function
.global mutex_unlock
mutex_unlock:
  SVC SVC_MUT_ULK
  bx lr

.type wait_until_next_period, %function
.global wait_until_next_period
wait_until_next_period:
  SVC SVC_WAIT
  bx lr

.global get_time
get_time:
  SVC SVC_TIME
  bx lr

.global get_priority
get_priority:
  SVC SVC_PRIORITY 
  bx lr

.global thread_time
thread_time:
  SVC SVC_THR_TIME
  bx lr



/* NEW SYSCALLS FOR TUD/HID */
.global user_tud_task
user_tud_task:
  SVC SVC_TUD_TASK
  bx lr

.global user_board_millis
user_board_millis:
  SVC SVC_BOARD_MILLIS
  bx lr

.global user_tud_hid_ready
user_tud_hid_ready:
  SVC SVC_TUD_HID_READY
  bx lr

.global user_tud_hid_report
user_tud_hid_report:
  SVC SVC_TUD_HID_REPORT
  bx lr


/* NEW SYSCALLS FOR BLINK PROCESSING */
.global user_blink_init
user_blink_init:
  SVC SVC_BLINK_INIT
  bx lr

.global user_blink_predict
user_blink_predict:
  SVC SVC_BLINK_PREDICT
  bx lr


/* UART READ */
.global uart_read
uart_read:
  SVC SVC_UART_READ
  bx lr


/* The following stubs are not required to be implemented */

.global _start
_start:
  SVC SVC_SCHD_START
  bx lr

.global _kill
_kill:
  SVC 0
  bx lr
  
.global _gettimeofday
_gettimeofday:
  SVC SVC_TIME
  bx lr

.global _times
_times:
  SVC SVC_THR_TIME
  bx lr

.global _getpid
_getpid:
  SVC 0
  bx lr

.thumb_func
.global idle_default
idle_default:
  bkpt
 
 
 