/**
 * @file adc.c
 *
 * @brief
 *
 * @date
 *
 * @author
 */

#include <gpio.h>
#include <stdint.h>
#include <rcc.h>
#include <unistd.h>
#include <adc.h>

/** @brief The ADC register map. */
struct adc_reg_map {
    volatile uint32_t SR;       /**< Status Register */
    volatile uint32_t CR1;      /**< Control Register 1 */
    volatile uint32_t CR2;      /**< Control Register 2 */
    volatile uint32_t SMPR1;    /**< Sample Time Register 1 */
    volatile uint32_t SMPR2;    /**< Sample Time Register 2 */
    volatile uint32_t JOFR1;    /**< Injected Channel Data Offset Register 1 */
    volatile uint32_t JOFR2;    /**< Injected Channel Data Offset Register 2 */
    volatile uint32_t JOFR3;    /**< Injected Channel Data Offset Register 3 */
    volatile uint32_t JOFR4;    /**< Injected Channel Data Offset Register 4 */
    volatile uint32_t HTR;      /**< Watchdog Higher Threshold Register */
    volatile uint32_t LTR;      /**< Watchdog Lower Threshold Register */
    volatile uint32_t SQR1;     /**< Regular Sequence Register 1 */
    volatile uint32_t SQR2;     /**< Regular Sequence Register 2 */
    volatile uint32_t SQR3;     /**< Regular Sequence Register 3 */
    volatile uint32_t JSQR;     /**< Injected Sequence Register */
    volatile uint32_t JDR1;     /**< Injected Data Register 1 */
    volatile uint32_t JDR2;     /**< Injected Data Register 2 */
    volatile uint32_t JDR3;     /**< Injected Data Register 3 */
    volatile uint32_t JDR4;     /**< Injected Data Register 4 */
    volatile uint32_t DR;       /**< Regular Data Register */
    volatile uint32_t CCR;      /**< Common Control Register */
};

/** @brief Base address for ADC */
#define ADC_BASE  (struct adc_reg_map *) 0x40012000

#define RCC_EN (1 << 8)         // RCC Enable
#define RES (1 << 24)           // Resolution
#define SWSTART (1 << 30)       // Conversion start
#define EOC (1 << 1)            // End of conversion

void adc_init() {
	/* ADC D11 (PA_7) */
    gpio_init(GPIO_A, 7, MODE_ANALOG_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW,
              PUPD_NONE, ALT0);

    struct adc_reg_map *adc = ADC_BASE;

    /* RCC Enable */
    struct rcc_reg_map *rcc = RCC_BASE;
    rcc->apb2_enr |= RCC_EN;

    adc->CR1 |= RES;     // 10 bit resolution (01)
    adc->CR2 |= 1;       // ADC On

	return;
}

uint16_t adc_read_chan(uint8_t chan){
    struct adc_reg_map *adc = ADC_BASE;

    adc->SQR3 = chan;               // Set channel
    adc->CR2 |= SWSTART;            // Start conversion

    while (!(adc->SR & EOC));         // Wait for end of conversion

    uint16_t data = adc->DR;
	return data;
}