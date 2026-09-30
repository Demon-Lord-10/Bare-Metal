#include "rcc.h"
#include "uart.h"

void Uart_init(){
    USART_Init(USART2,115200UL);
}

int main(){
    SystemClockInit();
    Uart_init();
    while(1){
        for(volatile int i=0;i<1000000UL;i++);
        USART_Printf(USART2, "Hello\r\n");
    }
    return 0;
}
