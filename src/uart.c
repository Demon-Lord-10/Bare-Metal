#include "uart.h"

GPIO_Config USART1_TX = {
    .pin = 9,               /* PA9 */
    .mode = GPIO_MODE_AFM,
    .af = 7,
    .otype = GPIO_OTYPE_PP,
    .speed = GPIO_SPEED_MEDIUM,
    .pull = GPIO_PUPD_PU
};

GPIO_Config USART1_RX = {
    .pin = 10,              /* PA10 */
    .mode = GPIO_MODE_AFM,
    .af = 7,
    .otype = GPIO_OTYPE_PP,
    .speed = GPIO_SPEED_MEDIUM,
    .pull = GPIO_PUPD_PU
};

GPIO_Config USART2_TX = {
    .pin = 2,               /* PA2 */
    .mode = GPIO_MODE_AFM,
    .af = 7,
    .otype = GPIO_OTYPE_PP,
    .speed = GPIO_SPEED_MEDIUM,
    .pull = GPIO_PUPD_PU
};

GPIO_Config USART2_RX = {
    .pin = 3,               /* PA3 */
    .mode = GPIO_MODE_AFM,
    .af = 7,
    .otype = GPIO_OTYPE_PP,
    .speed = GPIO_SPEED_MEDIUM,
    .pull = GPIO_PUPD_PU
};

GPIO_Config USART6_TX = {
    .pin = 11,              /* PA11 */
    .mode = GPIO_MODE_AFM,
    .af = 8,                /* USART6 is A8*/
    .otype = GPIO_OTYPE_PP,
    .speed = GPIO_SPEED_MEDIUM,
    .pull = GPIO_PUPD_PU
};

GPIO_Config USART6_RX = {
    .pin = 12,              /* PA12 */
    .mode = GPIO_MODE_AFM,
    .af = 8,
    .otype = GPIO_OTYPE_PP,
    .speed = GPIO_SPEED_MEDIUM,
    .pull = GPIO_PUPD_PU
};

uint32_t USART_GetBaudRate(uint32_t baudrate){
    return (pclk + (baudrate / 2)) / baudrate;
}


void USART_Init(USART_TypeDef *USART ,uint32_t baudrate){

    if (USART == USART1) {
        RCC_APB2_Enable(RCC_APB2ENR_USART1EN);
        GPIO_Init(GPIOA, &USART1_TX);
        GPIO_Init(GPIOA, &USART1_RX);
    } else if (USART == USART6) {
        RCC_APB2_Enable(RCC_APB2ENR_USART6EN);
        GPIO_Init(GPIOA, &USART6_TX);
        GPIO_Init(GPIOA, &USART6_RX);
    } else {
        RCC_APB1_Enable(RCC_APB1ENR_USART2EN);
        GPIO_Init(GPIOA, &USART2_TX);
        GPIO_Init(GPIOA, &USART2_RX);
    }

    USART->CR1 &= ~(USART_CR1_M);
    USART->BRR = USART_GetBaudRate(baudrate);
    USART->CR1 |= (USART_CR1_TE | USART_CR1_RE |USART_CR1_UE);
}

void USART_WriteByte(USART_TypeDef *USART, uint8_t data){
    while (!(USART->SR & USART_SR_TXE));                //Wait till transmit data register is empty
    USART->DR = data;
    //while (!(USART->SR & USART_SR_TC));                 //Wait for full transmission complete -->not needed
}

uint8_t USART_ReadByte(USART_TypeDef *USART){
    while (!(USART->SR & USART_SR_RXNE));               //Wait until a byte has been received
    return (uint8_t)(USART->DR & 0xFF);
}

void USART_Write(USART_TypeDef *USART, const char *str, uint32_t size){
    for (uint32_t i = 0; i < size; i++) {
        USART_WriteByte(USART, (uint8_t)str[i]);
    }
}
