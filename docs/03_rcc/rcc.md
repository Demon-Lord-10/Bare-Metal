# Reset and Clock Control (RCC)

## 1. Overview

The **Reset and Clock Control (RCC)** peripheral is the heartbeat of the STM32 microcontroller. On modern ARM Cortex-M microcontrollers, power efficiency is a primary architectural goal. Consequently, **every peripheral is clock-gated (powered off) by default**.

Before you can read or write to any peripheral register — whether it is a GPIO port, a timer, an ADC, or a UART controller — you must first enable its clock through the RCC. Attempting to access a peripheral's registers while its clock is disabled results in either silent failure (writes are ignored) or a **BusFault / HardFault**.

In our example we will set the system clock(HCLK) to be 24MHz.

---

## 2. The STM32F401 Clock Tree

The STM32F401CCU6 can operate at clock frequencies up to **84 MHz**. To achieve this, it features several internal and external clock sources routed through a Phase-Locked Loop (PLL).

![clock.png](clock.png)

### 2.1 Clock Sources

| Source | Name | Typical Frequency | Characteristics |
|---|---|---|---|
| **HSI** | High-Speed Internal | 16 MHz | Built-in RC oscillator. Available instantly at power-up; slightly temperature-sensitive. |
| **HSE** | High-Speed External | 4–26 MHz (25 MHz on BlackPill) | External quartz crystal. High accuracy, required for stable USB and precision timing. |
| **PLL** | Phase-Locked Loop | Up to 84 MHz | Multiplies and divides HSI or HSE to generate higher system frequencies. |
| **LSI** (Low-Speed Internal) | ~32 kHz | Internal low-power RC | Internal RC oscillator. Low-power, low-cost, but low accuracy. Used for IWDG and AWU; keeps running in Stop/Standby mode. |
| **LSE** (Low-Speed External) | 32.768 kHz | External watch crystal | External watch crystal. Low-power but highly accurate. Used for RTC clock/calendar functions. |

#### HSE (High Speed External Clock):
The high speed external clock signal (HSE) can be generated from two possible clock sources:
- HSE external crystal/ceramic resonator
- HSE external user clock
The resonator and the load capacitors have to be placed as close as possible to the oscillator pins in order to minimize output distortion and startup stabilization time. The loading capacitance values must be adjusted according to the selected oscillator.

#### HSI (High Speed Internal Clock):
The HSI clock signal is generated from an internal 16 MHz RC oscillator and can be used directly as a system clock, or used as PLL input.
The HSI RC oscillator has the advantage of providing a clock source at low cost (no external components). It also has a faster startup time than the HSE crystal oscillator however, even with calibration the frequency is less accurate than an external crystal oscillator or ceramic resonator.

Note: The default clock is HSI if not configured.

#### PLL (Phase Locked Loop):
The PLL is used to generate a higher-speed system clock from a lower-frequency input clock source. It takes either HSI or HSE as its input reference clock and multiplies it up to produce a higher output frequency, allowing the microcontroller to run at its maximum system clock speed even though the input oscillators (HSI/HSE) run at lower frequencies.
- PLL input source can be selected as either HSI or HSE (via a configurable input MUX/divider).
- The input clock is divided and then multiplied by configurable factors(down below) to produce the desired PLL output frequency.
- The PLL output can then be selected as the system clock (SYSCLK).
- Using the PLL allows flexibility — a low-cost or low-power source like HSI can still drive the system at high speed.
- The PLL requires a short lock time to stabilize before its output can be reliably used as the system clock.

Note: PLL isn't multiply-only — it has input/output dividers too, so it can divide as well as multiply.

#### LSE (Low Speed External Clock):
The LSE clock is generated using a 32.768 kHz low speed external crystal or ceramic resonator. It has the advantage of providing a low-power but highly accurate clock source to the real-time clock peripheral (RTC) for clock/calendar or other timing functions.

#### LSI (Low Speed Internal Clock):
The LSI RC acts as a low-power clock source that can be kept running in Stop and Standby mode for the independent watchdog (IWDG) and Auto-wakeup unit (AWU). The clock frequency is around 32 kHz.

### 2.3 SYSCLK vs. HCLK vs. FCLK (Cortex Clock)
SYSCLK: The System Clock (SYSCLK) serves as the primary clock source for the microcontroller. It can be sourced from various inputs like the internal HSI, external HSE, or a PLL. SYSCLK determines the clock speed for the AHB bus after passing through the AHB Prescaler.

HCLK: The High-Speed Clock (HCLK) is essentially SYSCLK after it has been divided by the AHB Prescaler. HCLK is crucial because it feeds several critical components such as the Cortex core, the AHB bus, memory interfaces, and DMA controllers.

FCLK: The Cortex Clock (FCLK) is the clock source specifically for the processor core. Generally, FCLK is directly derived from HCLK, meaning they often run at the same frequency. However, under certain low-power scenarios or other special conditions, FCLK may differ from HCLK.


---

## 3. Bus Architecture & Peripheral Mapping

Peripherals are organized onto distinct internal buses according to their required bandwidth and clock speed:

```
  Bus      Max Speed   Peripherals Connected
 ──────   ─────────── ─────────────────────────────────────────────────────────
  AHB1      84 MHz     GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOH, RCC, DMA1, DMA2
  AHB2      84 MHz     USB OTG FS
  APB1      42 MHz     TIM2, TIM3, TIM4, TIM5, USART2, I2C1, I2C2, I2C3, SPI2, SPI3, WWDG, PWR
  APB2      84 MHz     TIM1, TIM9, TIM10, TIM11, USART1, USART6, SPI1, SPI4, ADC1, SYSCFG, EXTI
```

!!! important "Bus Gating Rule"
    To configure a peripheral, identify which bus it resides on, then set the corresponding enable bit in that bus's clock register (e.g., `RCC->AHB1ENR` for GPIO, `RCC->APB1ENR` for I2C, `RCC->APB2ENR` for USART1).

---

## 4. Key RCC Registers

### 4.1 `RCC_CR` — Clock Control Register

![rcc](RCC_CR.png)

Controls oscillators and monitors their stability flags:

| Bit(s) | Name | Description |
|---|---|---|
| 0 | `HSION` | Turn on HSI oscillator (1 = ON) |
| 1 | `HSIRDY` | HSI ready flag (1 = Stable) |
| 16 | `HSEON` | Turn on HSE oscillator (1 = ON) |
| 17 | `HSERDY` | HSE ready flag (1 = Stable) |
| 24 | `PLLON` | Turn on Main PLL (1 = ON) |
| 25 | `PLLRDY` | Main PLL ready flag (1 = Locked and stable) |

### 4.2 `RCC_PLLCFGR` — PLL Configuration Register

![rcc1](RCC_PLLCFGR.png)

Formulates the PLL output clock using four dividers/multipliers:


| Field | Name | Bits | Purpose |
|---|---|---|---|
| `PLLSRC` | Source Select | 22 | `0` = HSI, `1` = HSE |
| `PLLM` | Division Factor M | 5:0 | Divides input clock to 1–2 MHz (recommended 1 MHz) |
| `PLLN` | Multiplication Factor N | 14:6 | Multiplies VCO frequency ($192 \le N \le 432$) |
| `PLLP` | Main System Division P | 17:16 | Divides VCO to system clock (`00`=/2, `01`=/4, `10`=/6, `11`=/8) |
| `PLLQ` | USB OTG FS Division Q | 27:24 | Divides VCO to 48 MHz for USB operations |

In our case we have PLLSRC is HSE and the frequeny is 25MHz and PLLM=25 PLLN=192 and PLLP=8 which gives the PLL frequency to be 24MHz.

### 4.3 `RCC_CFGR` — Clock Configuration Register

![rcc3](RCC_CFGR.png)

Selects which clock drives `SYSCLK` and configures bus prescalers:

| Field | Name | Bits | Purpose |
|---|---|---|---|
| `SW[1:0]` | System Clock Switch | 1:0 | `00` = HSI, `01` = HSE, `10` = PLL |
| `SWS[1:0]` | Switch Status (Read-Only) | 3:2 | Indicates active system clock (`00` = HSI, `01` = HSE, `10` = PLL) |
| `HPRE[3:0]` | AHB Prescaler | 7:4 | `0xxx` = Not divided, `1000` = /2, `1001` = /4, etc. |
| `PPRE1[2:0]` | APB1 Prescaler | 12:10 | `0xx` = Not divided, `100` = /2 (APB1 max is 42 MHz!) |
| `PPRE2[2:0]` | APB2 Prescaler | 15:13 | `0xx` = Not divided, `100` = /2 |

### 4.4 `RCC_AHB1ENR` — AHB1 Peripheral Clock Enable

![rcc4](RCC_AHB1ENR.png)

Enables peripheral clocks on AHB1.

#### **Why are we setting the clock frequency to 24MHz?**

Since our HCLK < 30 MHz, we don't need any wait cycles, and therefore no FLASH_ACR wait state configuration is required. This keeps SystemClockInit() simpler — at higher SYSCLK frequencies (e.g. 84 MHz), you'd need to set the appropriate flash latency bits before switching SYSCLK to the PLL, otherwise the CPU can fetch corrupted instructions. Staying under 30 MHz lets us skip that step entirely.

![Wait](Wait.png)

## 5. Driver Implementation

### 5.1 Initializing System Clock (`SystemClockInit`)

This function transitions the CPU from the default internal 16 MHz HSI to an external 25 MHz crystal stabilized and multiplied by the PLL to target higher operational frequencies.

```c
void SystemClockInit(void) {
    // 1. Turn on External High-Speed Oscillator (HSE)
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)); // Wait until HSE is stable

    // 2. Configure PLL factors (Source = HSE, M = 25, N = 192, P = 8)
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN
                    | RCC_PLLCFGR_PLLP   | RCC_PLLCFGR_PLLQ);

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE
                 | (25u  << RCC_PLLCFGR_PLLM_Pos)
                 | (192u << RCC_PLLCFGR_PLLN_Pos)
                 | (3u   << RCC_PLLCFGR_PLLP_Pos);

    // 3. Set Bus Prescalers (AHB = /1, APB1 = /1, APB2 = /1)
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV1 | RCC_CFGR_PPRE2_DIV1;

    // 4. Turn on the PLL and wait for lock
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 5. Switch System Clock (SYSCLK) to PLL output
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    // 6. Wait until hardware confirms PLL is the active system clock source
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}
```


**Why are wait loops (`while`) necessary?** 
Physical oscillators take milliseconds to stabilize their vibration frequency, and the PLL analog feedback loop takes microseconds to lock phase. Switching the CPU clock to an unready oscillator causes immediate code execution lockup. The hardware signals readiness via `HSERDY` and `PLLRDY`.

### 5.2 Enabling GPIO Port Clocks (`RCC_GPIOClockEnable`)

Because GPIO ports are independent blocks on AHB1, we dynamically enable the port clock by matching the port pointer:

```c
void RCC_GPIOClockEnable(GPIO_TypeDef *port) {
    if (port == GPIOA)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    else if (port == GPIOB)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    else if (port == GPIOC)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    else if (port == GPIOD)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    else if (port == GPIOE)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    else if (port == GPIOH)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
}
```

Tracing the clock path: Looking at the clock tree diagram, the path a peripheral clock takes is: System Clock MUX → SYSCLK → AHB Prescaler → HCLK. From HCLK, the clock branches out to power the Cortex core, the AHB bus, memory, and DMA directly, while also feeding the APB1 and APB2 buses (via their respective prescalers) which in turn drive peripheral modules like timers, USART, and I2C.

Because a peripheral clock has to propagate through this whole chain before it's actually live at the peripheral, there's a small but real delay between setting the enable bit and the clock signal reaching the peripheral.

Also note we are not clearing the bits since there is only one bit so we can just OR it but for other cases we need to clear for more than 1 bit and then OR it.

!!! tip "Hardware Delay After Clock Enable"
    According to the STM32 Cortex-M4 programming guidelines, after setting a bit in an enable register (such as `AHB1ENR`), a delay of at least two peripheral bus cycles is required before accessing the peripheral's registers. In practice, performing a dummy read guarantees the bus has synchronized:
    ```c
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    volatile uint32_t dummy = RCC->AHB1ENR; // Ensures clock is active before register writes
    ```

### 5.3 Generic Clock Enable Helper (`RCC_ClockEnable`)

For arbitrary peripherals across various buses, a generic pointer-mask helper enables straightforward configuration:

```c
void RCC_ClockEnable(volatile uint32_t *enr, uint32_t mask) {
    *enr |= mask;
}
```

---

## 6. Key Takeaways

!!! tip "Key Takeaways"
    1. **Clock Gating by Default**: All peripheral hardware starts in a powered-off state. You must explicitly set the corresponding bit in `RCC_AHBxENR` or `RCC_APBxENR`.
    2. **Wait for Ready Flags**: Never switch clock sources without verifying the respective `*RDY` flag in `RCC_CR`.
    3. **Respect Bus Limits**: APB1 maximum frequency is **42 MHz**, whereas AHB and APB2 can reach **84 MHz**.
    4. **Bus Synchronization**: Allow a small delay or execute a dummy read on the enable register before modifying peripheral registers.
	5. **Wait States**: Flash wait states must be configured whenever HCLK exceeds the safe read threshold (30 MHz at typical VDD) — staying below it, as with our 24 MHz setup, lets you skip FLASH_ACR configuration entirely.