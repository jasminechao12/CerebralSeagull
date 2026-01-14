/** @file   syscall_thread.c
 *
 *  @brief  
 *
 *  @date   
 *
 *  @author 
 */

#include <stdint.h>
#include <stdbool.h>
#include "syscall.h"
#include "syscall_thread.h"
#include "syscall_mutex.h"
#include <systick.h>
#include <printk.h>
#include <arm.h>
#include <mpu.h>


/** @brief      Initial XPSR value, all 0s except thumb bit. */
#define XPSR_INIT 0x1000000

/** @brief Interrupt return code to user mode using PSP.*/
#define LR_RETURN_TO_USER_PSP 0xFFFFFFFD
/** @brief Interrupt return code to kernel mode using MSP.*/
#define LR_RETURN_TO_KERNEL_MSP 0xFFFFFFF1 // never use

/**
 * @brief      Heap high and low pointers.
 */
//@{
extern char
  __thread_u_stacks_low,
  __thread_u_stacks_top,
  __thread_k_stacks_low,
  __thread_k_stacks_top;
//@}

/**
 * @brief      Precalculated values for UB test.
 */
float ub_table[] = {
  0.000, 1.000, .8284, .7798, .7568,
  .7435, .7348, .7286, .7241, .7205,
  .7177, .7155, .7136, .7119, .7106,
  .7094, .7083, .7075, .7066, .7059,
  .7052, .7047, .7042, .7037, .7033,
  .7028, .7025, .7021, .7018, .7015,
  .7012, .7009
};

/* array of tcb_t's */
tcb_t threads[16];
/* array of kmutex_t's*/
kmutex_t mutexes[32];
/* index of the curr_thread */
uint8_t curr_thread;
/* maximum created threads */
uint8_t maxc_threads;
/* maximum created mutexes */
uint8_t maxc_mutexes;
/* num threads scheduled */
uint8_t threads_scheduled;
/* running ub count */
float ub;


extern void* thread_kill;

/* Stack vars */
uint32_t calculated_stack_size;

char* msp = &__thread_k_stacks_top; // top of kernel stack, msp 
char* psp = &__thread_u_stacks_top; // top of user stack, psp



// Default idle function if none is provided
void default_idle_fn() {
  wait_for_interrupt();
}


// Context_ptr is a ptr to the current kernel stack frame
void *pendsv_c_handler(void *context_ptr) {

  threads[curr_thread].svc_status = get_svc_status();

  uint8_t next_thread = find_next_thread_rms();

  set_svc_status(threads[next_thread].svc_status);

  threads[curr_thread].k_frame = context_ptr;

  // Running to runnable
  if ((threads[curr_thread].status == 1) && (curr_thread != next_thread)) {
    threads[curr_thread].status = 0;
  }

  curr_thread = next_thread;
  threads[curr_thread].status = 1;  // running

  return threads[curr_thread].k_frame;
} 






void init_tcb(uint8_t i) {
  // TCB
  threads[i].set = false;
  threads[i].priority = -1;
  threads[i].dynamic_prio = -1;
  threads[i].period = 0;
  threads[i].comp_time = 0;
  threads[i].status = 3;
  threads[i].svc_status = 0;
  threads[i].count = 0;
  threads[i].total_count = 0;
  threads[i].total_running_time = 0;

  // stack
  threads[i].msp_top = msp - (i * calculated_stack_size);
  threads[i].psp_top = psp - (i * calculated_stack_size);
  threads[i].k_frame = (kernel_stack_frame *)(threads[i].msp_top - sizeof(kernel_stack_frame));
  threads[i].k_frame->u_frame = (interrupt_stack_frame *)(threads[i].psp_top - sizeof(interrupt_stack_frame));

  // kernel stack
  threads[i].k_frame->r4 = 0;
  threads[i].k_frame->r5 = 0;
  threads[i].k_frame->r6 = 0;
  threads[i].k_frame->r7 = 0;
  threads[i].k_frame->r8 = 0;
  threads[i].k_frame->r9 = 0;
  threads[i].k_frame->r10 = 0;
  threads[i].k_frame->r11 = 0;
  threads[i].k_frame->r14 = LR_RETURN_TO_USER_PSP;

  // user stack
  threads[i].k_frame->u_frame->r0 = 0;
  threads[i].k_frame->u_frame->r1 = 0;
  threads[i].k_frame->u_frame->r2 = 0;
  threads[i].k_frame->u_frame->r3 = 0;
  threads[i].k_frame->u_frame->r12 = 0;
  threads[i].k_frame->u_frame->lr = (uint32_t) &thread_kill;
  threads[i].k_frame->u_frame->pc = 0;
  threads[i].k_frame->u_frame->xPSR = XPSR_INIT;
}





int sys_thread_init(
  uint32_t max_threads,
  uint32_t stack_size,
  void *idle_fn,
  uint32_t max_mutexes
) {

  /* Stacks */
  calculated_stack_size = 1 << mm_log2ceil_size(stack_size * 4); 

  // Check if stacks are valid
  uint32_t total_stack_size = calculated_stack_size * max_threads;
  if ((&__thread_u_stacks_top - total_stack_size <= &__thread_u_stacks_low) || (&__thread_k_stacks_top - total_stack_size <= &__thread_k_stacks_low)) {
    return -1;
  }


  /* Initalize threads */
  for (int i = 0; i < 16; i++) {
    init_tcb(i);
  }

  /* Default function */
  threads[14].set = 1;
  threads[14].priority = 14;
  threads[14].status = 0;
  threads[14].k_frame->r14 = LR_RETURN_TO_USER_PSP;  // return to user mode

  /* Idle function */
  threads[15].set = 1;
  threads[15].priority = 15;
  threads[15].status = 0;
  if (idle_fn == NULL) {
    threads[15].k_frame->u_frame->pc = (uint32_t)&default_idle_fn;
  }
  else {
    threads[15].k_frame->u_frame->pc = (uint32_t)idle_fn;
  }

  /* Global vars */
  curr_thread = 14;
  maxc_threads = max_threads; 
  threads_scheduled = 0;
  ub = 0;

  /* Mutexes */
  maxc_mutexes = max_mutexes;
  for (int i = 0; i < 32; i++) {
    mutexes[i].created = false;
    mutexes[i].index = i;
    mutexes[i].locked = false;
    mutexes[i].locked_by = -1;
    mutexes[i].prio_ceil = -1;
  }

  return 0;
}






int sys_thread_create(
  void *fn,
  uint32_t prio,
  uint32_t C,
  uint32_t T,
  void *vargp
){
  // Error check if thread is alr created
  if (threads[prio].set) {
    return -1;
  }
  if (!(threads[prio].status == 3)) {
    return -1;
  }

  // Check if thread is scheduleable under UB Test
  float temp_ub = ub + ((float)C / (float)T);
  bool schedulable = (temp_ub <= ub_table[threads_scheduled + 1]);

  if (!schedulable) {   // Not scheduable
    return -1;
  }
  ub = temp_ub;
  threads_scheduled++;

  init_tcb(prio);


  // Thread is scheduable, scheduling it
  threads[prio].set = true;

  threads[prio].priority = prio;
  threads[prio].period = T;
  threads[prio].comp_time = C;

  threads[prio].status = 0; 
  threads[prio].svc_status = 0; // inactive

  threads[prio].k_frame->u_frame->pc = (uint32_t)fn;
  
  threads[prio].k_frame->u_frame->r0 = (uint32_t)vargp;

  return 0;
}





// Current thread is done, find next thread
uint8_t find_next_thread_rms() {
  int set_threads = 0;

  uint32_t next_thread_to_run = 16;

  // Increment down from highest priority -> lowest priority threads
  for (uint32_t i = 0; i < maxc_threads; i++) {
    // Find dynamic thread to run
    if (threads[i].set && (threads[i].dynamic_prio != (uint32_t)-1)) {
      if (threads[i].dynamic_prio <= next_thread_to_run) next_thread_to_run = threads[i].priority;
    }

    // Find static thread to run
    if ((threads[i].set && (threads[i].status == 0)) || (i == curr_thread && threads[i].status == 1)) {
      if (i < next_thread_to_run) next_thread_to_run = i;
    }

    if (threads[i].set) {
      set_threads++;
    }
  }
  if (next_thread_to_run != 16) return next_thread_to_run;



  // No thread available, set to default thread
  if (set_threads == 0) {
    threads[14].status = 1;
    return 14;
  }
  // Set to idle thread
  threads[15].status = 1;
  return 15;
}






/* Start the timer and run first thread */
int sys_scheduler_start( uint32_t frequency ){
  
  // Systick Timer
  systick_init(frequency);

  // pendSV
  pend_pendsv();

  return 0;
}

/* Current threads priority */
uint32_t sys_get_priority(){
  if (threads[curr_thread].dynamic_prio != (uint32_t)-1)
    return threads[curr_thread].dynamic_prio;
  return threads[curr_thread].priority;
}





/* Total System Time */
uint32_t sys_get_time(){
  return total_ticks;
}




/* Time the current thread has run */
uint32_t sys_thread_time(){
  return threads[curr_thread].total_running_time;
}



// Free space of thread (when terminated and finishes running)
void sys_thread_kill(){

  // default
  if (curr_thread == 14) {
    threads[curr_thread].status = 0;
    sys_exit(0);
  } 
  // idle
  else if (curr_thread == 15) {
    threads[curr_thread].status = 0;
    threads[curr_thread].k_frame->u_frame->pc = (uint32_t)&default_idle_fn;
  }
  else {
    ub -= ((float)threads[curr_thread].comp_time / (float)threads[curr_thread].period);
    threads_scheduled--;

    threads[curr_thread].status = 3;
    threads[curr_thread].set = false;

    // find new thread to run
    pend_pendsv();
  }
  
}




void sys_wait_until_next_period(){
  threads[curr_thread].status = 2;
  pend_pendsv();
}





kmutex_t *sys_mutex_init( uint32_t max_prio ) {
  for (uint32_t i = 0; i < maxc_mutexes; i++) {
    if (mutexes[i].created == false) {
      mutexes[i].created = true;
      mutexes[i].prio_ceil = max_prio;
      return &(mutexes[i]);
    }
  }
  return NULL;
}

void sys_mutex_lock( kmutex_t *mutex ) {
  if (mutex->locked) {
    printk("Warning: Trying to lock locked mutex, thread: %d\n", curr_thread);
    threads[curr_thread].status = 2;
    pend_pendsv();
  }
  // Lock mutex
  else if ((curr_thread >= mutex->prio_ceil) && (threads[curr_thread].status == 1)) {
    mutex->locked = true;
    mutex->locked_by = curr_thread;

    uint32_t new_dynamic_prio = ((mutex->prio_ceil < curr_thread) ? mutex->prio_ceil : curr_thread);
    threads[curr_thread].dynamic_prio = (new_dynamic_prio < threads[curr_thread].dynamic_prio) ? new_dynamic_prio : threads[curr_thread].dynamic_prio;

    pend_pendsv();
  }
  else {
    sys_thread_kill();
  }
}

// Helper to find dynamic priority
uint32_t find_curr_dynamic_prio() {
  for (int i = 0; i < maxc_mutexes; i++) {
    if (mutexes[i].locked_by == curr_thread) {
      return (mutexes[i].prio_ceil < curr_thread) ? mutexes[i].prio_ceil : curr_thread;
    }
  }
  return -1;
}

void sys_mutex_unlock( kmutex_t *mutex ) {
  if (mutex->locked) {
    mutex->locked = false;
    mutex->locked_by = -1;
    threads[curr_thread].status = 1;
    threads[curr_thread].dynamic_prio = find_curr_dynamic_prio();
  }
  else {
    printk("Warning: Trying to unlock unlocked mutex, thread: %d\n", curr_thread);
  }

  pend_pendsv();
}
