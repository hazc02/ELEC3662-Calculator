#include <stdint.h>
#include <stdbool.h>
#include "clock.h"
#include "gpio.h"
#include "lcd.h"
#include "keypad.h"
#include "calc.h"    // Our new Calculator Logic Module

#include <string.h>  // for memset
#include <stdio.h>   // for sprintf

int main(void)
{
    //  Initialise system and peripherals
    PLL_init();        // System clock to 80 MHz
    SysTick_init();    // SysTick timer
    GPIO_Init();       // Ports for LCD + Keypad
    LCD_Init();        // LCD in 4-bit mode

    // Initialise the calculator
    Calc_Init();

    LCD_Clear();
    LCD_String("Enter Expression:");

    while(1)
    {
        // Continuously read the keypad
        char key = Keypad_GetKey();

        if (key != '\0') {
            // For demonstration, display each pressed key on the second line
            LCD_SetCursor(1,0);
            LCD_String("Key: ");
            LCD_Data(key);

            // Clear command
            if (key == 'C') {
                LCD_Clear();
                LCD_String("Cleared!");
                Calc_ClearExpression();
                continue;
            }

            // Evaluate command
            else if (key == '=') {
                long result = Calc_Evaluate();

                if (Calc_HadError()) {
                    // If an error occurred (e.g. divide by zero)
                    LCD_Clear();
                    LCD_String("Error in expression!");
                } else {
                    // Display the final result
                    char buf[16];
                    sprintf(buf, "Result: %ld", result);
                    LCD_Clear();
                    LCD_String(buf);
                }
                continue;
            }
            
            // Otherwise, treat the key as a digit or operator
            else {
                // Try adding the character to the calculator buffer
                if (Calc_AddChar(key) < 0) {
                    // If buffer is full or an error occurred
                    LCD_Clear();
                    LCD_String("Buffer Full!");
                }
            }
        }
    }

    return 0;
}
