#include "uart.h"
#include <stdarg.h>
#include <stdint.h>

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

void USART_Printf(USART_TypeDef *USART, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt != '%') {
            USART_WriteByte(USART, (uint8_t)*fmt++);
            continue;
        }

        fmt++;

        switch (*fmt) {
            case 'c':{
                USART_WriteByte(USART,(uint8_t)va_arg(args,int));
                break;
            }
            case 's':{
                const char* s = va_arg(args,const char *);
                if (!s) s = "(null)";
                while (*s) USART_WriteByte(USART, (uint8_t)*s++);
                break;
            }
            case 'd':{
                int32_t val = va_arg(args,int32_t);
                char buf[11];
                int i=0;
                uint32_t u;

                if(val < 0){
                    USART_WriteByte(USART, '-');
                    u = (uint32_t)(-(val + 1)) + 1;
                }
                else{ 
                    u = (uint32_t)val;
                }
                if (u == 0) buf[i++] = '0';
                while (u > 0) {
                    buf[i++] = '0' + (u % 10);
                    u /= 10;
                }
                while (i--) USART_WriteByte(USART, buf[i]);
                break;
                }
            case 'u':{
                uint32_t val = va_arg(args,uint32_t);
                char buf[10];
                int i=0;

                if(val == 0) buf[i++] = '0';
                while(val>0){
                    buf[i++] = '0' + (val%10);
                    val/=10;
                }
                while(i--) USART_WriteByte(USART, buf[i]);
                break;
            }
            case '%':{ 
                USART_WriteByte(USART, '%'); 
                break;
            }
            case '\0':{
                      va_end(args);
                      return;
            }
            default:  /* unknown: print it as-is */
                      USART_WriteByte(USART, '%');
                      USART_WriteByte(USART, (uint8_t)*fmt);
                      break;
        }
        fmt++;
    }

    va_end(args);
}
