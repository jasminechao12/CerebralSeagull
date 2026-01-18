/**
* @file
*
* @brief
*
* @date
*
* @author
*/

#include <unistd.h>
#include <systick.h>
#include <syscall_thread.h>
#include <arm.h>

#define UNUSED __attribute__((unused))


/** @brief The systick register map. */
struct systick_reg_map {
    volatile uint32_t CTRL;   /*  Control Register       */
    volatile uint32_t LOAD;   /*  Load Register          */
    volatile uint32_t VAL;    /*  Current Value Register */
    volatile uint32_t CALIB;  /*  Calibration Register   */
};


/** @brief Base address for systick */
#define SYSTICK_BASE  (struct systick_reg_map *) 0xE000E010

#define UNUSED __attribute__((unused))
#define TIMER_EN (1)
#define TICK_EN (1 << 1)
#define CLKSOURCE (1 << 2)

volatile uint32_t total_ticks;

void systick_init(uint32_t frequency) {
    struct systick_reg_map *systick = SYSTICK_BASE;

    systick->LOAD = (CPU_FREQ / frequency) - 1;
    
    systick->VAL = 0;
    systick->CTRL = CLKSOURCE | TICK_EN | TIMER_EN;

    // Initialize global count to 0
    total_ticks = 0;
}

void systick_delay(uint32_t ticks) {
    // Poll until ticks elapsed
    uint32_t starting_ticks = total_ticks;
    while (total_ticks < starting_ticks + ticks);
    return;
}

uint32_t systick_get_ticks() {
    return total_ticks;
}

/* Update status of threads */
void systick_c_handler() {
    // Increment total_ticks and thread count
    total_ticks++;
    
    if (scheduler_started) {
        /* Update status of all threads */
        for (int i = 0; i < maxc_threads; i++){
            if (threads[i].set) {
                threads[i].total_count++;       // Increment how far in period thread is

                if (threads[i].status == 1) {
                    threads[i].count++;         // Increment how long thread has been running for
                    threads[i].total_running_time++;
                }

                uint32_t period = threads[i].period;
                // If the thread is waiting and new period happens 
                if ((threads[i].status == 2) && (threads[i].total_count >= period)) {
                    threads[i].status = 0;  // Set to runnable
                    threads[i].count = 0;   // Reset threads counter
                    threads[i].total_count = 0;
                }   
                // If thread is at comp_time, set to waiting 
                if ((threads[i].status == 1) && (threads[curr_thread].count == threads[curr_thread].comp_time)) {
                    threads[curr_thread].status = 2;
                }
            }   
        }
        
        pend_pendsv();
    }   

    return;
}
