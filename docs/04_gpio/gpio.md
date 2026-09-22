# General Purpose Input/Output (GPIO)

## 1. Overview

General Purpose Input/Output (GPIO) pins are the primary interface through which a microcontroller interacts with external electronics — reading buttons and sensors, driving status LEDs, controlling displays, or switching communication lines.

In the STM32F401, pins are grouped into **ports** labeled **A, B, C, D, E, and H**. Each port controls up to **16 physical pins** (labeled Pin 0 through Pin 15). Every pin is highly configurable and can be placed into one of four distinct functional modes:

1. **Digital Input**: Reads high or low voltage levels (e.g., push buttons, sensors).
2. **Digital Output**: Drives a pin to 3.3V or 0V (e.g., LEDs, relays, control signals).
3. **Alternate Function**: Re-routes the pin to an internal hardware peripheral (e.g., UART Tx/Rx, SPI SCK/MOSI, I2C SDA/SCL).
4. **Analog Mode**: Disconnects digital input Schmitt triggers to allow the Analog-to-Digital Converter (ADC) to sample continuous voltage levels without leakage.

---

## 2. Internal Architecture

Each pin contains internal pull-up/pull-down resistors, protection diodes, an input Schmitt trigger, and dual output transistors (P-MOS and N-MOS):

![gpio](gpio.png)

---

## 3. Hardware Registers

Every GPIO port has a dedicated block of memory-mapped registers at an address offset specified in the Reference Manual (RM0368):

| Register | Offset | Access | Description |
|---|---|---|---|
| `MODER` | `0x00` | Read/Write | Mode register (2 bits per pin) |
| `OTYPER` | `0x04` | Read/Write | Output type register (1 bit per pin) |
| `OSPEEDR` | `0x08` | Read/Write | Output speed / slew rate (2 bits per pin) |
| `PUPDR` | `0x0C` | Read/Write | Pull-up / pull-down resistor configuration (2 bits per pin) |
| `IDR` | `0x10` | Read-Only | Input data register (reflects pin voltages) |
| `ODR` | `0x14` | Read/Write | Output data register (software driven states) |
| `BSRR` | `0x18` | Write-Only | Bit Set/Reset Register (atomic pin manipulation) |
| `LCKR` | `0x1C` | Read/Write | Configuration lock register |
| `AFR[0]` | `0x20` | Read/Write | Alternate function low register (pins 0 to 7, 4 bits per pin) |
| `AFR[1]` | `0x24` | Read/Write | Alternate function high register (pins 8 to 15, 4 bits per pin) |

---

## 4. Configuration Modes

### 4.1 Pin Mode (`MODER`)

Each pin uses 2 bits to define its role:

| Value | Mode | Description |
|:---:|---|---|
| `00` | **Input** | Default state after reset. The pin senses voltage without driving. |
| `01` | **General Purpose Output** | Software directly controls pin voltage via `ODR` or `BSRR`. |
| `10` | **Alternate Function** | Internal peripheral (USART, SPI, TIM) drives and senses the pin. |
| `11` | **Analog** | Disconnects the digital buffer to save power and enable ADC sampling. |

### 4.2 Output Type: Push-Pull vs. Open-Drain (`OTYPER`)

- **Push-Pull (`0`)**: Both P-MOS and N-MOS transistors are active.
  - When driven **HIGH**, the pin actively connects to 3.3V (sources current).
  - When driven **LOW**, the pin actively connects to GND (sinks current).
  - *Standard choice for LEDs, digital communication, and motor drivers.*

- **Open-Drain (`1`)**: Only the bottom N-MOS transistor is active.
  - When driven **LOW**, the pin actively connects to GND.
  - When driven **HIGH**, the transistor turns off, leaving the pin **floating (High-Z)**.
  - An external or internal pull-up resistor is required to pull the line up to VDD.
  - *Essential for shared bus protocols like I2C or multi-drop lines where multiple devices share a line without short-circuit risk.*

### 4.3 Pull-Up / Pull-Down Resistors (`PUPDR`)

When a pin is configured as an input and nothing is connected to it, it is in a "floating" state and will oscillate unpredictably between 0 and 1 due to ambient electromagnetic noise.

- **`00` (None)**: Floating input; used when an external resistor or driver already biases the line.
- **`01` (Pull-Up)**: Activates an internal ~40 kΩ resistor connected to 3.3V. Pin reads `1` when idle.
- **`10` (Pull-Down)**: Activates an internal ~40 kΩ resistor connected to GND. Pin reads `0` when idle.

### 4.4 Bit Set/Reset Register (`BSRR`)

The `BSRR` is a 32-bit **write-only** register specifically designed to eliminate read-modify-write hazards.

```
 31                          16 15                           0
┌──────────────────────────────┬──────────────────────────────┐
│       Reset Bits (BRy)       │        Set Bits (BSy)        │
│   Writing 1 clears pin y     │     Writing 1 sets pin y     │
│   Writing 0 has NO effect    │    Writing 0 has NO effect   │
└──────────────────────────────┴──────────────────────────────┘
```

**Why use `BSRR` instead of `ODR`?**
If you use `ODR` to set a pin:
```c
port->ODR |= (1 << pin); // Read ODR -> Modify bit -> Write ODR
```
If an interrupt occurs between the Read and the Write and modifies another pin in the same port, the second pin's modification is overwritten and lost when the original code completes.

Writing directly to `BSRR`:
```c
port->BSRR = (1 << pin); // Single atomic machine instruction!
```
Because writing `0` does nothing, writing to pin $y$ leaves all other 15 pins completely untouched without reading first.

---

## 5. Driver Implementation

### 5.1 Initializing a Pin (`GPIO_Init`)

The initialization function sets up the pin's mode, speed, pull-up/pull-down configuration, output type, and optional alternate function mapping:

```c
void GPIO_Init(GPIO_TypeDef *port, const GPIO_Config *cfg) {
    // 1. Ensure the port's AHB1 clock is enabled
    RCC_GPIOClockEnable(port);

    // 2. Set Pin Mode in MODER (2 bits per pin)
    port->MODER  &= ~(0x3 << (cfg->pin * 2));
    port->MODER  |=  (cfg->mode << (cfg->pin * 2));

    // 3. Set Output Type in OTYPER (1 bit per pin)
    port->OTYPER &= ~(0x1 << cfg->pin);
    port->OTYPER |=  (cfg->otype << cfg->pin);

    // 4. Set Slew Rate / Speed in OSPEEDR (2 bits per pin)
    port->OSPEEDR &= ~(0x3 << (cfg->pin * 2));
    port->OSPEEDR |=  (cfg->speed << (cfg->pin * 2));

    // 5. Configure Pull-Up / Pull-Down in PUPDR (2 bits per pin)
    port->PUPDR &= ~(0x3 << (cfg->pin * 2));
    port->PUPDR |=  (cfg->pull << (cfg->pin * 2));

    // 6. If Alternate Function, configure AFR[0] (pins 0-7) or AFR[1] (pins 8-15)
    if (cfg->mode == GPIO_MODE_AFM) {
        uint8_t shift = (cfg->pin > 7) ? (cfg->pin - 8) : cfg->pin;
        if (cfg->pin < 8) {
            port->AFR[0] &= ~(0xF << (4 * shift));
            port->AFR[0] |=  (cfg->af << (4 * shift));
        } else {
            port->AFR[1] &= ~(0xF << (4 * shift));
            port->AFR[1] |=  (cfg->af << (4 * shift));
        }
    }
}
```

### 5.2 Writing to a Pin (`GPIO_WritePin`)

This function commands a physical pin to either logic high or logic low using the atomic `BSRR` register:

```c
void GPIO_WritePin(GPIO_TypeDef *port, uint8_t pin, uint8_t state) {
    if (state)
        port->BSRR = (1U << pin);        // Set pin HIGH (3.3V)
    else
        port->BSRR = (1U << (pin + 16)); // Reset pin LOW (0V)
}
```

**What actually happens when you call `GPIO_WritePin`?**

1. **When `state = 1`**:
   - The driver writes to bit `pin` in `BSRR` (bits 0–15).
   - The hardware drives the physical pin to **+3.3V (Logic HIGH)**.

2. **When `state = 0`**:
   - The driver writes to bit `(pin + 16)` in `BSRR` (bits 16–31).
   - The hardware drives the physical pin to **0V / GND (Logic LOW)**.

### 5.3 Active-High vs. Active-Low Circuits

How your circuit reacts to `GPIO_WritePin` depends entirely on whether it is wired **Active-High** or **Active-Low**:

**Case A: Active-High Circuit (Standard)**
```
  Pin ───▶ [ Resistor ] ───▶ [ LED Anode (+) ] ───▶ [ Cathode (-) ] ───▶ GND
```
- `GPIO_WritePin(port, pin, 1)` → Output is 3.3V → Voltage across LED → **Turns ON**
- `GPIO_WritePin(port, pin, 0)` → Output is 0V → No voltage difference → **Turns OFF**

**Case B: Active-Low Circuit (e.g., STM32 BlackPill onboard LED on PC13)**
```
  +3.3V ───▶ [ Resistor ] ───▶ [ LED Anode (+) ] ───▶ [ Cathode (-) ] ───▶ Pin
```
- `GPIO_WritePin(port, pin, 0)` → Output is 0V → Pin sinks current to GND → **Turns ON!**
- `GPIO_WritePin(port, pin, 1)` → Output is 3.3V → Both sides at 3.3V → **Turns OFF!**

### 5.4 Reading a Pin (`GPIO_ReadPin`)

Reads the digital voltage level present on the pin via the `IDR` register:

```c
uint8_t GPIO_ReadPin(GPIO_TypeDef *port, uint8_t pin) {
    if (pin < 16)
        return (uint8_t)((port->IDR >> pin) & 0x1U);

    return 0xFF; // Error code for invalid pin number
}
```

### 5.5 Toggling a Pin (`GPIO_TogglePin`)

Inverts the current output state:

```c
void GPIO_TogglePin(GPIO_TypeDef *port, uint8_t pin) {
    if (pin >= 16) return;
    
    // Toggle the bit in ODR using bitwise XOR
    port->ODR ^= (1U << pin);
}
```

---

## 6. Key Takeaways

!!! tip "Key Takeaways"
    1. **Two bits per pin for mode**: `MODER` uses 2 bits per pin. Pin 13 occupies bits 26 and 27 (`13 * 2`).
    2. **Atomic writes via `BSRR`**: Always use `BSRR` for setting/resetting output pins. Avoid read-modify-write on `ODR` to prevent race conditions during interrupts.
    3. **Active-Low awareness**: Check your board's schematic before assuming `WritePin(..., 1)` turns a peripheral on.
    4. **Floating inputs are dangerous**: Always configure `PUPDR` (Pull-Up or Pull-Down) on floating input pins to avoid erratic Schmitt trigger transitions.
