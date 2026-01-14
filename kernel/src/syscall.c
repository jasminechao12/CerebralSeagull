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
#include <syscall.h>
#include <arm.h>
#include <uart.h>
#include <printk.h>

#define UNUSED __attribute__((unused))



extern char __heap_low;
extern char __heap_top;

// static char *heap = &__heap_low;
static uint32_t heap = (uint32_t)&__heap_low;

// Increment program data space by incr bytes
// On success, return to where the pointer originally was 
// On error, return -1
void *sys_sbrk(int incr){
  if (heap + incr <= (uint32_t)&__heap_top) {
    heap = heap + incr;
    return (void *)(heap - incr);
  }
  else {
    return (void *)-1;
  }
}


// Write to stdout, return 0 on success, return -1 on error
int sys_write(int file, char *ptr, int len){
  // Only allow writes to stdout
  if (file != 1) return -1;

  for (int i = 0; i < len; i++) {
    // uart_put_byte(*ptr);
    while (uart_put_byte(ptr[i]) == -1);
  }

  return len;
}


// Read bytes from stdin, one by one and echo.
// Ascii value 4 returns immediately
// \b do not write into ptr, but remove the previous character in ptr
// \n newline (syscall should return with number of characters read into the buffer plus newline)
int sys_read(int file, char *ptr, int len){
  // Only allow reads from stdin
  if (file != 0) return -1;

  int chars_read = 0;
  char *i = ptr;

  while (chars_read < len) {
    char c;

    while(uart_get_byte(&c));

    if (c == 0x4) {
      return chars_read;
    } 
    else if (c == '\b') {
      if (chars_read > 0) {
        chars_read--;
        i--;
      }
      uart_put_byte('\b');
      uart_put_byte(' ');
      uart_put_byte('\b');
    } 
    else if (c == '\n') {
      *i = '\n';
      i++;
      uart_put_byte('\n');
      chars_read++;
      return chars_read;
    }
    else if (c == '\r') {
      *i = '\n';
      i++;
      uart_put_byte('\r');
      uart_put_byte('\n');
      chars_read++;
      return chars_read;
    }
    else {
      *i = c;
      i++;
      uart_put_byte(c);
      chars_read++;
    }
  }
  return chars_read;
}



#define RXNEIE (1 << 5)
#define TXEIE (1 << 7)

void sys_exit(int status){
  printk("%d", status);

  disable_interrupts();

  uart_flush();

  while (1);
}
