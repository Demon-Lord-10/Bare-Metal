#include "gpio.h"

void GPIO_Init(GPIO_TypeDef *port, const GPIO_Config *cfg){

    RCC_GPIOClockEnable(port);

    port->MODER  &= ~(3UL<< cfg->pin*2UL);
    port->MODER  |=  (cfg->mode << cfg->pin*2UL);

    port->OTYPER &= ~(1UL<< cfg->pin);
    port->OTYPER |= (cfg->otype << cfg->pin);

    port->OSPEEDR &= ~(3UL<< cfg->pin*2UL);
    port->OSPEEDR |= (cfg->speed << cfg->pin*2UL);

    port->PUPDR &= ~(3UL<< cfg->pin*2UL);
    port->PUPDR |= (cfg->pull << cfg->pin*2UL);

    if(cfg->mode == GPIO_MODE_AFM){
        uint8_t shift = (cfg->pin>7UL)? cfg->pin-8UL : cfg->pin;
        if((cfg->pin<8UL)){
            port->AFR[0] &= ~(0xF << (4UL*(shift)));
            port->AFR[0] |= (cfg->af << (4UL*shift));
        }
        else{
            port->AFR[1] &= ~(0xF << (4UL*shift));
            port->AFR[1] |= (cfg->af << (4UL*shift));
        }
    }
}

void GPIO_WritePin(GPIO_TypeDef *port, uint8_t pin, uint8_t state){
    if (pin >= 16UL) return;
    if(state) 
        port->BSRR = (1UL<<pin);
    else
        port->BSRR = (1UL<<(pin+16UL));
}

uint8_t GPIO_ReadPin(GPIO_TypeDef *port, uint8_t pin)
{
    if(pin<16UL)
        return (0x1 & (port->IDR>>pin));

    return 0xFF;
}
void GPIO_TogglePin(GPIO_TypeDef *port, uint8_t pin){
    if (pin >= 16UL) return;
    if (port->ODR & (1UL << pin))
        port->BSRR = (1UL << (pin + 16UL));
    else
        port->BSRR = (1Ul << pin);
}

void RCC_GPIOClockEnable(GPIO_TypeDef *port){
    if(port == GPIOA)
        RCC_AHB1_Enable(RCC_AHB1ENR_GPIOAEN);
    else if(port == GPIOB)
        RCC_AHB1_Enable(RCC_AHB1ENR_GPIOBEN);
    else if(port == GPIOC)
        RCC_AHB1_Enable(RCC_AHB1ENR_GPIOCEN);
    else if(port == GPIOD)
        RCC_AHB1_Enable(RCC_AHB1ENR_GPIODEN);
    else if(port == GPIOE)
        RCC_AHB1_Enable(RCC_AHB1ENR_GPIOEEN);
    else if(port == GPIOH)
        RCC_AHB1_Enable(RCC_AHB1ENR_GPIOHEN);
}
