#include "clock.h"
#include "gpio.h"
#include "lcd.h"
#include "keypad.h"

int main(void) {
    char key;

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
    LCD_String("Press a key:");

    while (1) {
        key = Keypad_GetKey();
        if (key != '\0') {
            LCD_Clear();
            LCD_String("Key Pressed: ");
            LCD_Data(key);
        }
    }
    return 0;
}
