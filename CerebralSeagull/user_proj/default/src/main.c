#include <349_threads.h>
#include <349_lib.h>
#include <peripherals.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define UNUSED __attribute__((unused))

/** @brief thread user space stack size - 1KB */
#define USR_STACK_WORDS 512
#define NUM_THREADS 2
#define NUM_MUTEXES 1
#define CLOCK_FREQUENCY 1000


/** @brief Computation time of the task */
#define THREAD_C0_MS 1
#define THREAD_C1_MS 1
// #define THREAD_C2_MS 6

/** @brief Period of the thread */
#define THREAD_T0_MS 2
#define THREAD_T1_MS 10
// #define THREAD_T2_MS 40



volatile float shared_sample = 0;
volatile bool sample_ready = false;

typedef struct __attribute__((packed)) {
    int8_t x;
    int8_t y;
    uint8_t buttons;
} joy_report_t;



// Connect USB device
void thread_0( UNUSED void *vargp ) {
  while (1) {
    user_tud_task();
    wait_until_next_period();
  }
}


// Testing HID
// void thread_1(UNUSED void *vargp) {
//   joy_report_t rpt = {0};
//   int8_t delta = 5;

//   while (1) {
//     rpt.x += delta;
//     if (rpt.x >= 100 || rpt.x <= -100) delta = -delta;

//     if (user_tud_hid_ready()) { 
//       user_tud_hid_report(0, &rpt, sizeof(rpt)); 
//     }
//     wait_until_next_period();
//   }
// }



// UART Reading Thread - reads single byte (0 or 1)
void thread_1(UNUSED void *vargp) {
  UNUSED mutex_t *eeg_mutex = ( mutex_t * ) vargp;

  char c;
  joy_report_t rpt = {0};
  static bool button_pressed = false;
  
  while (1) {
    // If button was pressed in previous iteration, release it now
    if (button_pressed) {
      if (user_tud_hid_ready()) {
        rpt.buttons = 0x00;
        user_tud_hid_report(0, &rpt, sizeof(rpt));
        button_pressed = false;
      }
    }
    
    // Check for new UART data
    if (uart_read(&c) == 0) {
      uint8_t blink_status = (uint8_t)c;
      
      // If blink detected (status == 1), send HID button press
      if (blink_status == 1 && !button_pressed) {
        if (user_tud_hid_ready()) {
          // Press button
          rpt.buttons = 0x01;
          user_tud_hid_report(0, &rpt, sizeof(rpt));
          button_pressed = true;  // Will be released in next iteration
        }
      }
    }
    wait_until_next_period();
  }
}



// Data Processing Thread
// void thread_2(UNUSED void *vargp) {
//   mutex_t *eeg_mutex = ( mutex_t * ) vargp;

//   user_blink_init();
  
//   joy_report_t rpt = {0};
//   int inference_skip = 0;

//   while (1) {
//     mutex_lock(eeg_mutex);
//     if (sample_ready) {
//       float local_sample = shared_sample;
//       sample_ready = false;
//       mutex_unlock(eeg_mutex);

//       float prob = user_blink_predict(local_sample);
      
//       inference_skip++;
//       if (inference_skip >= 25) {
//         if (prob > 0.80f) {
//             // 3. Perform HID action
//             rpt.buttons = 0x01;
//             user_tud_hid_report(0, &rpt, sizeof(rpt));
                        
//             rpt.buttons = 0x00;
//             user_tud_hid_report(0, &rpt, sizeof(rpt));
//         }
//         inference_skip = 0;
//       }
//     } else {
//       mutex_unlock(eeg_mutex);
//     }
//     wait_until_next_period();
//   }
// }

void idle_thread() {
  while( 1 );
}




int main(UNUSED int argc, UNUSED char const *argv[]){
  
  ABORT_ON_ERROR( thread_init( NUM_THREADS, USR_STACK_WORDS, &idle_thread, NUM_MUTEXES ) );

  mutex_t *eeg_mutex = mutex_init( 0 );
  if ( eeg_mutex == NULL ) {
    printf( "Failed to create mutex 0\n" );
    return -1;
  }

  printf( "Successfully initialized threads...\n" );

  ABORT_ON_ERROR( thread_create( &thread_0, 0, THREAD_C0_MS, THREAD_T0_MS, NULL ) );

  ABORT_ON_ERROR( thread_create( &thread_1, 1, THREAD_C1_MS, THREAD_T1_MS, ( void * )eeg_mutex ) );

  // ABORT_ON_ERROR( thread_create( &thread_2, 2, THREAD_C2_MS, THREAD_T2_MS, ( void * )eeg_mutex ) );

  printf( "Successfully created threads! Starting scheduler...\n" );

  ABORT_ON_ERROR( scheduler_start( CLOCK_FREQUENCY ), "Threads are unschedulable!\n" );

  while (1);

  return 0;
}
