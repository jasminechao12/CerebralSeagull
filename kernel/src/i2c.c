#include <unistd.h>

#include <gpio.h>
#include <i2c.h>
#include <rcc.h>
#include <printk.h>


/** @brief The I2C register map. */
struct I2C_reg_map {
    volatile uint32_t CR1;   /**<  Control Register 1 */
    volatile uint32_t CR2;   /**<  Control Register 2 */
    volatile uint32_t OAR1;  /**<  Own Addr. Register 1 */
    volatile uint32_t OAR2;  /**<  Own Addr. Register 2 */
    volatile uint32_t DR;    /**<  Data Register */
    volatile uint32_t SR1;   /**<  Status Register 1 */
    volatile uint32_t SR2;   /**<  Status Register 2 */
    volatile uint32_t CCR;   /**<  Clock Control Register */
    volatile uint32_t TRISE; /**<  Rise Time Register */
    volatile uint32_t FLTR;  /**<  Filter Register */
};

/** @brief Base address for I2C */
#define I2C_BASE  (struct I2C_reg_map *) 0x40005400

#define RCC_EN (1 << 21)
#define ACK_EN (1 << 10)
#define I2C_START (1 << 8)
#define I2C_STOP (1 << 9)
#define TXE (1 << 7)
#define BTF (1 << 2)
#define ADDR (1 << 1)


void i2c_master_init(uint16_t clk){
    /* SCL (PB_8) */
    gpio_init(GPIO_B, 8, MODE_ALT, OUTPUT_OPEN_DRAIN, OUTPUT_SPEED_LOW, 
                PUPD_NONE, ALT4);
    /* SDA (PB_9) */
    gpio_init(GPIO_B, 9, MODE_ALT, OUTPUT_OPEN_DRAIN, OUTPUT_SPEED_LOW, 
                PUPD_NONE, ALT4);

    struct I2C_reg_map *i2c = I2C_BASE;

    /* RCC Enable */
    struct rcc_reg_map *rcc = RCC_BASE;
    rcc->apb1_enr |= RCC_EN;
    
    i2c->CR2 |= 0b010000;           // Peripheral clock freq (16MHz)
    i2c->CCR = clk;                 // Clock control

    i2c->CR1 |= ACK_EN;             // Acknowledge enable
    i2c->CR1 |= 1;                  // Peripheral enable
    return;
}

void i2c_master_start() {
    struct I2C_reg_map *i2c = I2C_BASE;
    i2c->CR1 |= I2C_START;          // Start Generation
    while (!(i2c->SR1 & 1));        // EV5 (Waiting for SB to be asserted)
    return;
}

void i2c_master_stop() {
    struct I2C_reg_map *i2c = I2C_BASE;

    // EV8_2 (Waiting for TxE and BTF to be asserted)
    while (!((i2c->SR1 & TXE) && (i2c->SR1 & BTF)));

    i2c->CR1 |= I2C_STOP;           // Stop Generation
    return;
}

int i2c_master_write(uint8_t *buf, uint16_t len, uint8_t slave_addr){
    struct I2C_reg_map *i2c = I2C_BASE;

    i2c->DR = slave_addr << 1;      // Slave address

    while(!(i2c->SR1 & ADDR));      // EV6 (Waiting for addr to be sent)
    (void)i2c->SR2;

    while(!(i2c->SR1 & TXE));       // EV8_1 (Waiting for data register to be empty)

    /* Sending Data */
    for (int i = 0; i < len; i++) {
        // EV8 (Waiting for data register to be empty)
        while(!(i2c->SR1 & TXE));
        i2c->DR = buf[i];
    }

    return 0;
}

// Don't implement
int i2c_master_read(uint8_t *buf, uint16_t len, uint8_t slave_addr){
    (void) buf;
    (void) len;
    (void) slave_addr;

    return 0;
}