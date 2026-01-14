/** @file syscall_thread.h
 *
 *  @brief  Custom syscalls to support thread library.
 *
 *  @date   March 27, 2019
 *
 *  @author Ronit Banerjee <ronitb@andrew.cmu.edu>
 */

#ifndef _SYSCALL_THREAD_H_
#define _SYSCALL_THREAD_H_

#include <stdint.h>
#include <stdbool.h>
#include "syscall_mutex.h"
#include <systick.h>

typedef struct {
  uint32_t r0;   /** @brief Register value for r0 */
  uint32_t r1;   /** @brief Register value for r1 */
  uint32_t r2;   /** @brief Register value for r2 */
  uint32_t r3;   /** @brief Register value for r3 */
  uint32_t r12;  /** @brief Register value for r12 */
  uint32_t lr;   /** @brief Register value for lr*/ // where we go after (thread_kill)
  uint32_t pc;   /** @brief Register value for pc */
  uint32_t xPSR; /** @brief Register value for xPSR */ 
} interrupt_stack_frame;

typedef struct {
  interrupt_stack_frame* u_frame;   /** @brief user_frame */
  uint32_t r4;   /** @brief Register value for r4 */
  uint32_t r5;   /** @brief Register value for r5 */
  uint32_t r6;   /** @brief Register value for r6 */
  uint32_t r7;   /** @brief Register value for r7 */
  uint32_t r8;  /** @brief Register value for r8 */
  uint32_t r9;   /** @brief Register value for r9 */
  uint32_t r10;   /** @brief Register value for r10 */
  uint32_t r11;   /** @brief Register value for r11 */
  uint32_t r14;   /** @brief Register value for r14 (link reg)*/ //return code
} kernel_stack_frame;

typedef struct {
  bool set;                    // the thread has been set or not
  uint32_t priority;            // thread's static priority
  uint32_t dynamic_prio;        // thread's dynamic priority
  uint32_t period;	             // how long thread can go
  uint32_t comp_time;           // allocated computation time
  uint32_t status;              // runnable(0), running(1), waiting(2) or empty(3)
  uint32_t svc_status;          // SVC status 
  uint32_t count;               // Run counter (how long it's been running)
  uint32_t total_count;         // How long it's been initialized
  uint32_t total_running_time;  // How long thread has been running for
  char* msp_top;               // top of the msp 
  char* psp_top;               // top of the psp
  kernel_stack_frame* k_frame; // kernel stack frame

} tcb_t;


/* STATIC VARIABLES */

/* array of tcb_t's */
extern tcb_t threads[16];
/* index to the curr_thread */
extern uint8_t curr_thread;
/* maximum created threads */
extern uint8_t maxc_threads;
/* num threads scheduled */
extern uint8_t threads_scheduled;

/**
 * @brief      The PendSV interrupt handler.
 */
void *pendsv_c_handler( void * );

/**
 * @brief      Initialize the thread library
 *
 *             A user program must call this initializer before attempting to
 *             create any threads or starting the scheduler.
 *
 * @param[in]  max_threads        Maximum number of threads that will be
 *                                created.
 * @param[in]  stack_size         Declares the size in words of all user and
 *                                kernel stacks created.
 * @param[in]  idle_fn            Pointer to a thread function to run when no
 *                                other threads are runnable. If NULL is
 *                                is supplied, the kernel will provide its
 *                                own idle function that will sleep.
 * @param[in]  max_mutexes        Maximum number of mutexes that will be
 *                                created.
 *
 * @return     0 on success or -1 on failure
 */
int sys_thread_init(
  uint32_t        max_threads,
  uint32_t        stack_size,
  void           *idle_fn,
  uint32_t        max_mutexes
);

/**
 * @brief      Create a new thread running the given function. The thread will
 *             not be created if the UB test fails, and in that case this function
 *             will return an error.
 *
 * @param[in]  fn     Pointer to the function to run in the new thread.
 * @param[in]  prio   Priority of this thread. Lower number are higher
 *                    priority.
 * @param[in]  C      Real time execution time (scheduler ticks).
 * @param[in]  T      Real time task period (scheduler ticks).
 * @param[in]  vargp  Argument for thread function (usually a pointer).
 *
 * @return     0 on success or -1 on failure
 */
int sys_thread_create( void *fn, uint32_t prio, uint32_t C, uint32_t T, void *vargp );

/**
 * @brief      Allow the kernel to start running the thread set.
 *
 *             This function should enable SysTick and thus enable your
 *             scheduler. It will not return immediately unless there is an error.
 *			   It may eventually return successfully if all thread functions are
 *   		   completed or killed.
 *
 * @param[in]  frequency  Frequency (Hz) of context swaps.
 *
 * @return     0 on success or -1 on failure
 */
int sys_scheduler_start( uint32_t frequency );

/**
 * @brief      Get the current time.
 *
 * @return     The time in ticks.
 */
uint32_t sys_get_time( void );

/**
 * @brief      Get the effective priority of the current running thread
 *
 * @return     The thread's effective priority
 */
uint32_t sys_get_priority( void );

/**
 * @brief      Gets the total elapsed time for the thread (since its first
 *             ever period).
 *
 * @return     The time in ticks.
 */
uint32_t sys_thread_time( void );

/**
 * @brief      Waits efficiently by descheduling thread.
 */
void sys_wait_until_next_period( void );

/**
* @brief      Kills current running thread. Aborts program if current thread is
*             main thread or the idle thread or if current thread exited
*             while holding a mutex.
*
* @return     Does not return.
*/
void sys_thread_kill( void );

/**
 * @brief      Get the current time.
 *
 * @return     The time in ticks.
 */
uint32_t sys_get_time( void );

/**
 * @brief      Get the effective priority of the current running thread
 *
 * @return     The thread's effective priority
 */
uint32_t sys_get_priority( void );

/**
 * @brief      Gets the total elapsed time for the thread (since its first
 *             ever period).
 *
 * @return     The time in ticks.
 */
uint32_t sys_thread_time( void );

/**
 * @brief      Waits efficiently by descheduling thread.
 */
void sys_wait_until_next_period( void );

/**
* @brief      Kills current running thread. Aborts program if current thread is
*             main thread or the idle thread or if current thread exited
*             while holding a mutex.
*
* @return     Does not return.
*/
void sys_thread_kill( void );

#endif /* _SYSCALL_THREAD_H_ */


uint8_t find_next_thread_round_robin( void );
uint8_t find_next_thread_rms( void );
