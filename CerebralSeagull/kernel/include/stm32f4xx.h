#ifndef STM32F4XX_H
#define STM32F4XX_H

#include <stdint.h>

// 1. Hardware Constants for STM32F401
#define USB_OTG_FS_PERIPH_BASE       (0x50000000UL)
#define USB_OTG_FS_MAX_IN_ENDPOINTS  4
#define USB_OTG_FS_TOTAL_FIFO_SIZE   1280 // bytes

// 2. IRQ Mapping
typedef enum {
  OTG_FS_IRQn = 67
} IRQn_Type;

// 3. System Clock (Set this to your CPU frequency, e.g., 16MHz)
#define SystemCoreClock              16000000UL

// 4. Standard ARM Macros/Stubs
#define SET_BIT(REG, BIT)            ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)          ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)           ((REG) & (BIT))
#define __NOP()                      __asm__ volatile ("nop")

// 5. Connect TinyUSB's NVIC calls to your own nvic_irq function
// Assuming your nvic_irq(num, 1) enables and nvic_irq(num, 0) disables
extern void nvic_irq(uint32_t irq, uint8_t enable);

static inline void NVIC_EnableIRQ(IRQn_Type irqn)  { nvic_irq((uint32_t)irqn, 1); }
static inline void NVIC_DisableIRQ(IRQn_Type irqn) { nvic_irq((uint32_t)irqn, 0); }

#endif