// gpio.c

#include "gpio.h"
#include "clock.h"

void GPIO_Init(void) {
    volatile unsigned long delay;
    
    // Enable clocks for GPIO Ports A and B
    SYSCTL_RCGCGPIO_R |= 0x03;  // Ports A (0x01), B (0x02)
    delay = SYSCTL_RCGCGPIO_R;  // Allow time for clock to stabilize

    // Delay for a short period to ensure ports are ready
    delay_us(5);

    // Port A Configuration (PA2 - EN, PA3 - RS for LCD Control)
    GPIO_PORTA_LOCK_R = 0x4C4F434B;     // Unlock GPIOCR register
    GPIO_PORTA_CR_R |= 0x0C;            // Allow changes to PA3-PA2
    GPIO_PORTA_AMSEL_R &= ~0x0C;        // Disable analog function
    GPIO_PORTA_PCTL_R &= ~0x0000FF00;   // GPIO function
    GPIO_PORTA_DIR_R |= 0x0C;           // PA3-PA2 outputs
    GPIO_PORTA_AFSEL_R &= ~0x0C;        // Regular function
    GPIO_PORTA_DEN_R |= 0x0C;           // Enable digital I/O

    // Port B Configuration (PB0-PB3 for LCD Data)
    GPIO_PORTB_LOCK_R = 0x4C4F434B;     // Unlock GPIOCR register
    GPIO_PORTB_CR_R |= 0x0F;            // Allow changes to PB3-PB0
    GPIO_PORTB_AMSEL_R &= ~0x0F;        // Disable analog function
    GPIO_PORTB_PCTL_R &= ~0x0000FFFF;   // GPIO function
    GPIO_PORTB_DIR_R |= 0x0F;           // PB3-PB0 outputs
    GPIO_PORTB_AFSEL_R &= ~0x0F;        // Regular function
    GPIO_PORTB_DEN_R |= 0x0F;           // Enable digital I/O
}
