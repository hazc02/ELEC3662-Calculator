#include "gpio.h"
#include "clock.h"

/*
GPIO Initialisation:

- Port A:
  - PA2: EN (Enable) control pin for the LCD.
  - PA3: RS (Register Select) control pin for the LCD.
	
- Port B:
  - PB0-PB3: Data pins connected to LCD DB4-DB7.

Configuration steps:
1. Enable clocks for the ports and allow time for stabilization.
2. Unlock GPIOCR registers to allow modifications.
3. Set pins as outputs where necessary.
4. Disable analog functionality to enable digital operation.
5. Set pins for GPIO function (no alternate functions).
6. Enable digital functionality for the required pins.
*/

void GPIO_Init(void) {
    volatile unsigned long delay;

    // Enable clocks for GPIO Ports A and B
    SYSCTL_RCGCGPIO_R |= 0x03; // Enable clock for Port A (0x01) and Port B (0x02)
    delay = SYSCTL_RCGCGPIO_R; // Allow time for clock to stabilise

    // Delay for a short period to ensure ports are stable
    delay_us(5);

    // Configure GPIO Port A (PA2 - EN, PA3 - RS for LCD Control)
    GPIO_PORTA_LOCK_R = 0x4C4F434B;      // Unlock GPIOCR register
    GPIO_PORTA_CR_R |= 0x0C;             // Allow changes to PA3-PA2
    GPIO_PORTA_AMSEL_R &= ~0x0C;         // Disable analog functionality
    GPIO_PORTA_PCTL_R &= ~0x0000FF00;    // Set PA2-PA3 for GPIO function
    GPIO_PORTA_DIR_R |= 0x0C;            // Set PA2-PA3 as outputs
    GPIO_PORTA_AFSEL_R &= ~0x0C;         // Disable alternate functions
    GPIO_PORTA_DEN_R |= 0x0C;            // Enable digital functionality for PA2-PA3

    // Configure GPIO Port B (PB0-PB3 for LCD Data)
    GPIO_PORTB_LOCK_R = 0x4C4F434B;      // Unlock GPIOCR register
    GPIO_PORTB_CR_R |= 0x0F;             // Allow changes to PB3-PB0
    GPIO_PORTB_AMSEL_R &= ~0x0F;         // Disable analog functionality
    GPIO_PORTB_PCTL_R &= ~0x0000FFFF;    // Set PB0-PB3 for GPIO function
    GPIO_PORTB_DIR_R |= 0x0F;            // Set PB0-PB3 as outputs
    GPIO_PORTB_AFSEL_R &= ~0x0F;         // Disable alternate functions
    GPIO_PORTB_DEN_R |= 0x0F;            // Enable digital functionality for PB0-PB3
}


