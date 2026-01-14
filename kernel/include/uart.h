/**
 * @file 
 *
 * @brief      
 *
 * @date       
 *
 * @author     
 */

#ifndef _UART_H_
#define _UART_H_

#include <unistd.h>
#include <stddef.h>


/** @brief The UART register map. */
struct uart_reg_map {
  volatile uint32_t SR;   /**< Status Register */
  volatile uint32_t DR;   /**<  Data Register */
  volatile uint32_t BRR;  /**<  Baud Rate Register */
  volatile uint32_t CR1;  /**<  Control Register 1 */
  volatile uint32_t CR2;  /**<  Control Register 2 */
  volatile uint32_t CR3;  /**<  Control Register 3 */
  volatile uint32_t GTPR; /**<  Guard Time and Prescaler Register */
};

/** @brief Base address for UART2 */
#define UART2_BASE  (struct uart_reg_map *) 0x40004400

// UART Register Bits
#define UART_EN (1 << 13)
#define RCC_EN (1 << 17)
#define TX_EN (1 << 2)
#define RX_EN (1 << 3)
#define SR_TXE (1 << 7)
#define SR_RXNE (1 << 5)
#define RXNEIE (1 << 5)
#define TXEIE (1 << 7)
#define TC (1 << 6)

// Buffer size
#define BUFFER_SIZE 1

typedef struct {
  char buffer[BUFFER_SIZE];
  size_t size;
  char *get;
  char *put;
} CircularBuffer;




void uart_init(int baud);

int uart_put_byte(char c);

int uart_get_byte(char *c);

int uart_write( int file, char *ptr, int len );

int uart_read(int file, char *ptr, int len );

void uart_flush();

#endif /* _UART_H_ */
