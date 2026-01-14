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
#include <rcc.h>
#include <uart.h>
#include <nvic.h>
#include <gpio.h>
#include <arm.h>
#include <stdbool.h>
#include <stddef.h>

#define UNUSED __attribute__((unused))

static CircularBuffer tx_buffer;
static CircularBuffer rx_buffer;

/* Buffer Helper Functions */
void buffer_init(CircularBuffer *cb) {
  cb->size = 0;
  cb->get = cb->buffer;
  cb->put = cb->buffer;
}

bool buffer_empty(CircularBuffer *cb) {
  return cb->size == 0;
}

bool buffer_full(CircularBuffer *cb) {
  return cb->size == BUFFER_SIZE;
}

void buffer_write(CircularBuffer *cb, char c) {
  if (buffer_full(cb)) return;
  *(cb->put) = c;
  cb->put++;
  if (cb->put >= cb->buffer + BUFFER_SIZE) {
    int state = save_interrupt_state_and_disable();
    cb->put = cb->buffer;
    restore_interrupt_state(state);
  }
  cb->size++;
  return;
}

char buffer_read(CircularBuffer *cb) {
  if (buffer_empty(cb)) return '\0';
  char c = *(cb->get);
  cb->get++;
  if (cb->get >= cb->buffer + BUFFER_SIZE) {
    int state = save_interrupt_state_and_disable();
    cb->get = cb->buffer;
    restore_interrupt_state(state);
  }
  cb->size--;
  return c;
}




/* UART Implementation */
void uart_init(int baud){
  // Initialize Buffer
  buffer_init(&tx_buffer);
  buffer_init(&rx_buffer);

  /* TX  (PA_2) */
  gpio_init(GPIO_A, 2, MODE_ALT, OUTPUT_PUSH_PULL,  OUTPUT_SPEED_LOW,
              PUPD_NONE, ALT7);

  /* RX  (PA_3) */
  gpio_init(GPIO_A, 3, MODE_ALT, OUTPUT_PUSH_PULL,  OUTPUT_SPEED_LOW,
              PUPD_NONE, ALT7);

  struct uart_reg_map *uart = UART2_BASE;

  /* RCC Enable */
  struct rcc_reg_map *rcc = RCC_BASE;
  rcc->apb1_enr |= RCC_EN;

  uart->BRR = baud;

  /* Enable interrupts */
  nvic_irq(38, IRQ_ENABLE);
  uart->CR1 |= RXNEIE;

  /* Enable tx, rx, uart */
  uart->CR1 |= TX_EN;
  uart->CR1 |= RX_EN;
  uart->CR1 |= UART_EN;

  return;
}


int uart_put_byte(char c){
  struct uart_reg_map *uart = UART2_BASE;
  if (buffer_full(&tx_buffer)) {
    return -1;
  }

  buffer_write(&tx_buffer, c);

  uart->CR1 |= TXEIE;

  return 0;
}


int uart_get_byte(char *c){
  struct uart_reg_map *uart = UART2_BASE;
  if (buffer_empty(&rx_buffer)) {
    return -1;
  }

  *c = buffer_read(&rx_buffer);

  uart->CR1 |= RXNEIE;

  return 0;
}



/* IRQ Handlers */
void USART2_TX_IRQHandler() {
  struct uart_reg_map *uart = UART2_BASE;

  if (!buffer_empty(&tx_buffer)) {
    int state = save_interrupt_state_and_disable();
    uart->DR = buffer_read(&tx_buffer);
    restore_interrupt_state(state);
  }

  if (buffer_empty(&tx_buffer)) {
    uart->CR1 &= ~TXEIE;
  }

  return;
}

void USART2_RX_IRQHandler() {
  struct uart_reg_map *uart = UART2_BASE;

  char c = uart->DR;

  if (!(buffer_full(&rx_buffer))) {
      buffer_write(&rx_buffer, c);
  }

  if (buffer_full(&rx_buffer)) {
    uart->CR1 &= ~RXNEIE;
  }

  return;
}

void uart_irq_handler(){
  struct uart_reg_map *uart = UART2_BASE;

  if (uart->SR & SR_RXNE)
    USART2_RX_IRQHandler();
  if (uart->SR & SR_TXE)
    USART2_TX_IRQHandler();

  nvic_clear_pending(38);
}




/* Flush TX Buffer */
void uart_flush(){
  struct uart_reg_map *uart = UART2_BASE;

  while (!buffer_empty(&tx_buffer)) {
    while (!(uart->SR & SR_TXE));

    char c = buffer_read(&tx_buffer);
    uart->DR = c;
  }

  while (!(uart->SR & TC));
}