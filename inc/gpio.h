#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "rcc.h"

/* Memory & Bus Bases */
#define PERIPH_BASE           0x40000000UL
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)

/* AHB1 Peripherals */
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400UL)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800UL)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00UL)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000UL)
#define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00UL)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800UL)


/* Peripheral Pointers */
#define GPIOA                 ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB                 ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC                 ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD                 ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE                 ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOH                 ((GPIO_TypeDef *) GPIOH_BASE)

typedef struct{
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
}GPIO_TypeDef;

typedef enum {
    GPIO_MODE_INPUT  = 0x0,
    GPIO_MODE_OUTPUT = 0x1,
    GPIO_MODE_AFM    = 0x2,
    GPIO_MODE_ANALOG = 0x3,
}GPIO_Mode;

typedef enum {
  GPIO_OTYPE_PP = 0x0, /* push-pull */
  GPIO_OTYPE_OD = 0x1  /* open-drain*/
} GPIO_OType;

typedef enum {
  GPIO_SPEED_LOW = 0x0,
  GPIO_SPEED_MEDIUM = 0x1,
  GPIO_SPEED_HIGH = 0x2,
  GPIO_SPEED_VHIGH = 0x3
} GPIO_Speed;

typedef enum {
  GPIO_PUPD_NONE = 0x0,
  GPIO_PUPD_PU = 0x1,
  GPIO_PUPD_PD = 0x2
} GPIO_PuPd;

typedef struct {
    uint8_t     pin;
    GPIO_Mode   mode;
    GPIO_PuPd   pull;
    GPIO_Speed  speed;
    GPIO_OType  otype;
    uint8_t     af;
}GPIO_Config;


/*Functions*/
void GPIO_Init(GPIO_TypeDef *port, const GPIO_Config *cfg);
void GPIO_WritePin(GPIO_TypeDef *port, uint8_t pin, uint8_t state);
uint8_t GPIO_ReadPin(GPIO_TypeDef *port, uint8_t pin);
void GPIO_TogglePin(GPIO_TypeDef *port, uint8_t pin);
void RCC_GPIOClockEnable(GPIO_TypeDef *port);

#endif


