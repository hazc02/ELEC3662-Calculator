#include "gpio.h"
#include "clock.h"

/**
 * @brief Initialises the GPIO ports and pins used in the project.
 *
 * Configures the necessary GPIO ports for interfacing with the LCD and keypad.
 * Sets the direction registers, enables digital functionality, and configures pull-up/pull-down resistors as needed.
 *
 */

void GPIO_Init(void) {
    volatile unsigned long delay;

    // Enable clocks for GPIO Ports A, B, D, and E
    SYSCTL_RCGCGPIO_R |= 0x1D;  // Ports A (0x01), B (0x02), D (0x08), E (0x10)
    delay = SYSCTL_RCGCGPIO_R;  // Allow time for clock to stabilise

    // Delay for 5us while ports stabilise
    delay_us(5);

    // Port A
    // PA2 - EN (Enable for LCD)
    // PA3 - RS (Register Select for LCD)

    // Unlock Port A
    GPIO_PORTA_LOCK_R = 0x4C4F434B;     // Unlock GPIOCR register
    GPIO_PORTA_CR_R |= 0x0C;            // Allow changes to PA3-PA2

    // Configure PA3-PA2 as GPIO outputs
    GPIO_PORTA_AMSEL_R &= ~0x0C;        // Disable analogue function on PA3-PA2
    GPIO_PORTA_PCTL_R &= ~0x0000FF00;   // Clear PCTL bits for PA3-PA2
    GPIO_PORTA_DIR_R |= 0x0C;           // Set PA3-PA2 as outputs
    GPIO_PORTA_AFSEL_R &= ~0x0C;        // Disable alternate functions on PA3-PA2
    GPIO_PORTA_DEN_R |= 0x0C;           // Enable digital functionality on PA3-PA2

    // Port B
    // PB0-PB3 - DB4 to DB7 (Data lines for LCD)

    // Unlock Port B
    GPIO_PORTB_LOCK_R = 0x4C4F434B;     // Unlock GPIOCR register
    GPIO_PORTB_CR_R |= 0x0F;            // Allow changes to PB3-PB0

    // Configure PB3-PB0 as GPIO outputs
    GPIO_PORTB_AMSEL_R &= ~0x0F;        // Disable analogue function on PB3-PB0
    GPIO_PORTB_PCTL_R &= ~0x0000FFFF;   // Clear PCTL bits for PB3-PB0
    GPIO_PORTB_DIR_R |= 0x0F;           // Set PB3-PB0 as outputs
    GPIO_PORTB_AFSEL_R &= ~0x0F;        // Disable alternate functions on PB3-PB0
    GPIO_PORTB_DEN_R |= 0x0F;           // Enable digital functionality on PB3-PB0

    // Port D Initialisation
    // PD0-PD3 - Keypad Columns (Outputs)

    // Unlock Port D
    GPIO_PORTD_LOCK_R = 0x4C4F434B;     // Unlock GPIOCR register
    GPIO_PORTD_CR_R |= 0x0F;            // Allow changes to PD3-PD0

    // Configure PD3-PD0 as GPIO outputs
    GPIO_PORTD_AMSEL_R &= ~0x0F;        // Disable analogue function on PD3-PD0
    GPIO_PORTD_PCTL_R &= ~0x0000FFFF;   // Clear PCTL bits for PD3-PD0
    GPIO_PORTD_DIR_R |= 0x0F;           // Set PD3-PD0 as outputs
    GPIO_PORTD_AFSEL_R &= ~0x0F;        // Disable alternate functions on PD3-PD0
    GPIO_PORTD_DEN_R |= 0x0F;           // Enable digital functionality on PD3-PD0

    // Port E Initialisation
    // PE0-PE3 - Keypad Rows (Inputs with pull-down resistors)

    // Unlock Port E
    GPIO_PORTE_LOCK_R = 0x4C4F434B;     // Unlock GPIOCR register
    GPIO_PORTE_CR_R |= 0x0F;            // Allow changes to PE3-PE0

    // Configure PE3-PE0 as GPIO inputs
    GPIO_PORTE_AMSEL_R &= ~0x0F;        // Disable analogue function on PE3-PE0
    GPIO_PORTE_PCTL_R &= ~0x0000FFFF;   // Clear PCTL bits for PE3-PE0
    GPIO_PORTE_DIR_R &= ~0x0F;          // Set PE3-PE0 as inputs
    GPIO_PORTE_AFSEL_R &= ~0x0F;        // Disable alternate functions on PE3-PE0
    GPIO_PORTE_DEN_R |= 0x0F;           // Enable digital functionality on PE3-PE0
    GPIO_PORTE_PDR_R |= 0x0F;           // Enable pull-down resistors on PE3-PE0
}

 