#include <349_threads.h>
#include <349_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define UNUSED __attribute__((unused))

/** @brief thread user space stack size - 1KB */
#define USR_STACK_WORDS 256
#define NUM_THREADS 2
#define NUM_MUTEXES 0
#define CLOCK_FREQUENCY 1


// TODO: Change C and T times
/** @brief Computation time of the task */
#define THREAD_C0_MS 1
#define THREAD_C1_MS 1

/** @brief Period of the thread */
#define THREAD_T0_MS 3
#define THREAD_T1_MS 3



// TODO:
// Need 2 tasks
//    Task1: ML Model 
//    Task2: Sending data thru UART

void thread_0( UNUSED void *vargp ) {}

void thread_1( UNUSED void *vargp ) {}

void idle_thread() {
  while( 1 );
}


int main(UNUSED int argc, UNUSED char const *argv[]){
  ABORT_ON_ERROR( thread_init( NUM_THREADS, USR_STACK_WORDS, &idle_thread, NUM_MUTEXES ) );

  printf( "Successfully initialized threads...\n" );

  ABORT_ON_ERROR( thread_create( &thread_0, 0, THREAD_C0_MS, THREAD_T0_MS, NULL ) );

  ABORT_ON_ERROR( thread_create( &thread_1, 1, THREAD_C1_MS, THREAD_T1_MS, NULL ) );

  printf( "Successfully created threads! Starting scheduler...\n" );

  ABORT_ON_ERROR( scheduler_start( CLOCK_FREQUENCY ), "Threads are unschedulable!\n" );

  while (1);

  return 0;
}
