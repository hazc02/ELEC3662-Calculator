#include "clock.h"
#include "gpio.h"
#include "lcd.h"

int main(void) {
    // Initialise system clock to 80 MHz
    PLL_init();

    // Initialise SysTick timer
    SysTick_init();

    // Initialise GPIO ports
    GPIO_Init();

    // Initialise LCD
    LCD_Init();

    // Clear LCD and display test message
    LCD_Clear();
    LCD_String("Hello, World!");

    while (1) {
    }

    return 0;
}

