// main.c

#include "clock.h"
#include "gpio.h"
#include "lcd.h"

int main(void) {
    // Initialize system clock to 80 MHz
    PLL_init();

    // Initialize SysTick timer
    SysTick_init();

    // Initialize GPIO ports
    GPIO_Init();

    // Initialize LCD
    LCD_Init();

    // Clear LCD and display test message
    LCD_Clear();
    LCD_String("Hello, World!");

    while (1) {
        
    }

    return 0;
}
