#include "gpio.h"

GPIO_Config gpio_PC13 ={
    .pin = 13,
    .mode = GPIO_MODE_OUTPUT,
    .pull = GPIO_PUPD_NONE,
    .speed = GPIO_SPEED_HIGH,
    .otype = GPIO_OTYPE_PP,
};

int main(){

    GPIO_Init(GPIOC , &gpio_PC13);
    GPIO_WritePin(GPIOC,13,0);

    while(1);
    return 0;
}
