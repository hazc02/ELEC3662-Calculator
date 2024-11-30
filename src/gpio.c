#include "gpio.h"
#include "clock.h"

/*
GPIO Initialisation:

- Port A:
  - PA2: EN (Enable) control pin for the LCD.
  - PA3: RS (Register Select) control pin for the LCD.

- Port B:
  - PB0-PB3: Data pins connected to LCD DB4-DB7.

- Port D:
  - PD0-PD3: Keypad columns (COL1-COL4).

- Port E:
  - PE0-PE3: Keypad rows (ROW1-ROW4).

Configuration steps:
1. Enable clocks for the required ports and allow time for stabilisation.
2. Unlock GPIOCR registers to allow modifications where necessary.
3. Configure pins for their respective roles (input/output).
4. Disable analogue functionality to enable digital operation.
5. Set pins for GPIO function (no alternate functions).
6. Enable digital functionality for the required pins.
7. Enable internal pull-up resistors for the keypad row inputs.
*/

void GPIO_Init(void) {
    volatile unsigned long delay;

    // Enable clocks for GPIO Ports A, B, D, and E
    SYSCTL_RCGCGPIO_R |= 0x1B; // Ports A (0x01), B (0x02), D (0x08), E (0x10)
    delay = SYSCTL_RCGCGPIO_R; // Allow time for clock to stabilise

    // Delay for a short period to ensure ports are stable
    delay_us(5);

    // ===== Configure GPIO Port A (PA2 - EN, PA3 - RS for LCD Control) =====
    GPIO_PORTA_LOCK_R = 0x4C4F434B;      // Unlock GPIOCR register
    GPIO_PORTA_CR_R |= 0x0C;             // Allow changes to PA3-PA2
    GPIO_PORTA_AMSEL_R &= ~0x0C;         // Disable analogue functionality
    GPIO_PORTA_PCTL_R &= ~0x0000FF00;    // Set PA2-PA3 for GPIO function
    GPIO_PORTA_DIR_R |= 0x0C;            // Set PA2-PA3 as outputs
    GPIO_PORTA_AFSEL_R &= ~0x0C;         // Disable alternate functions
    GPIO_PORTA_DEN_R |= 0x0C;            // Enable digital functionality for PA2-PA3

    // ===== Configure GPIO Port B (PB0-PB3 for LCD Data) =====
    GPIO_PORTB_LOCK_R = 0x4C4F434B;      // Unlock GPIOCR register
    GPIO_PORTB_CR_R |= 0x0F;             // Allow changes to PB3-PB0
    GPIO_PORTB_AMSEL_R &= ~0x0F;         // Disable analogue functionality
    GPIO_PORTB_PCTL_R &= ~0x0000FFFF;    // Set PB0-PB3 for GPIO function
    GPIO_PORTB_DIR_R |= 0x0F;            // Set PB0-PB3 as outputs
    GPIO_PORTB_AFSEL_R &= ~0x0F;         // Disable alternate functions
    GPIO_PORTB_DEN_R |= 0x0F;            // Enable digital functionality for PB0-PB3

    // ===== Configure GPIO Port D (PD0-PD3 for Keypad Columns) =====
    GPIO_PORTD_LOCK_R = 0x4C4F434B;      // Unlock GPIOCR register
    GPIO_PORTD_CR_R |= KEYPAD_COL_MASK;  // Allow changes to PD3-PD0
    GPIO_PORTD_AMSEL_R &= ~KEYPAD_COL_MASK; // Disable analogue functionality
    GPIO_PORTD_PCTL_R &= ~0x0000FFFF;    // Set PD0-PD3 for GPIO function
    GPIO_PORTD_DIR_R |= KEYPAD_COL_MASK; // Set PD0-PD3 as outputs
    GPIO_PORTD_AFSEL_R &= ~KEYPAD_COL_MASK; // Disable alternate functions
    GPIO_PORTD_DEN_R |= KEYPAD_COL_MASK; // Enable digital functionality for PD0-PD3

    // ===== Configure GPIO Port E (PE0-PE3 for Keypad Rows) =====
    GPIO_PORTE_LOCK_R = 0x4C4F434B;      // Unlock GPIOCR register
    GPIO_PORTE_CR_R |= KEYPAD_ROW_MASK;  // Allow changes to PE3-PE0
    GPIO_PORTE_AMSEL_R &= ~KEYPAD_ROW_MASK; // Disable analogue functionality
    GPIO_PORTE_PCTL_R &= ~0x0000FFFF;    // Set PE0-PE3 for GPIO function
    GPIO_PORTE_DIR_R &= ~KEYPAD_ROW_MASK; // Set PE0-PE3 as inputs
    GPIO_PORTE_AFSEL_R &= ~KEYPAD_ROW_MASK; // Disable alternate functions
    GPIO_PORTE_PUR_R |= KEYPAD_ROW_MASK;  // Enable internal pull-up resistors
    GPIO_PORTE_DEN_R |= KEYPAD_ROW_MASK;  // Enable digital functionality for PE0-PE3
}
