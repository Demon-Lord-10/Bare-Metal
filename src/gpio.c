#include "gpio.h"

void GPIO_Init(GPIO_TypeDef *port, const GPIO_Config *cfg){

    port->MODER  &= ~(0x3<< cfg->pin*2);
    port->MODER  |=  (cfg->mode << cfg->pin*2);

    port->OTYPER &= ~(1<< cfg->pin);
    port->OTYPER |= (cfg->otype << cfg->pin);

    port->OSPEEDR &= ~(0x3 << cfg->pin*2);
    port->OSPEEDR |= (cfg->speed << cfg->pin*2);

    port->PUPDR &= ~(0x3 << cfg->pin*2);
    port->PUPDR |= (cfg->pull << cfg->pin*2);

    if(cfg->mode == GPIO_MODE_AFM){
        uint8_t shift = (cfg->pin>7)? cfg->pin-8 : cfg->pin;
        if((cfg->pin<8)){
            port->AFR[0] &= ~(0xF << (4*(shift)));
            port->AFR[0] |= (cfg->af << (4*shift));
        }
        else{
            port->AFR[1] &= ~(0xF << (4*shift));
            port->AFR[1] |= (cfg->af << (4*shift));
        }
    }
}
void GPIO_WritePin(GPIO_TypeDef *port, uint8_t pin, uint8_t state){
    if(state) 
        port->BSRR |= (0x1<<pin);
    else
        port->BSRR |= (0x1<<(pin+16));
}

uint8_t GPIO_ReadPin(GPIO_TypeDef *port, uint8_t pin)
{
    if(pin<16)
        return (0x1 & (port->IDR>>pin));

    return 0xFF;
}
void GPIO_TogglePin(GPIO_TypeDef *port, uint8_t pin){
    if (pin >= 16) return;
    if (port->ODR & (1 << pin))
        port->BSRR = (1 << (pin + 16));
    else
        port->BSRR = (1 << pin);
}

void RCC_GPIOClockEnable(GPIO_TypeDef *port){
    if(port == GPIOA)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    else if(port == GPIOB)
        RCC->APB1ENR |= RCC_AHB1ENR_GPIOBEN;
    else if(port == GPIOC)
        RCC->APB1ENR |= RCC_AHB1ENR_GPIOCEN;
    else if(port == GPIOD)
        RCC->APB1ENR |= RCC_AHB1ENR_GPIODEN;
    else if(port == GPIOE)
        RCC->APB1ENR |= RCC_AHB1ENR_GPIOEEN;
    else if(port == GPIOH)
        RCC->APB1ENR |= RCC_AHB1ENR_GPIOHEN;
}
