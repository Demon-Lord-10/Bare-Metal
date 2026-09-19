#include "rcc.h"

void SystemClockInit(void){
    
    //Turns on external oscillator
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));
    
    //Configuring PLL
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN
            | RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLQ);
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE
        | (25u  << RCC_PLLCFGR_PLLM_Pos)
        | (192u << RCC_PLLCFGR_PLLN_Pos)
        | (3u   << RCC_PLLCFGR_PLLP_Pos);

    //Configuring APB1 APB2 and AHB
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV1 | RCC_CFGR_PPRE2_DIV1;
    
    //Turning on the PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    //making the CPU switch to PLL
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}


void RCC_ClockEnable(volatile uint32_t *enr, uint32_t mask){
    *enr |= mask;
}

