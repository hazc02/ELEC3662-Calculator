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
    // ---- 1) Hardware & Peripheral Initialisations ----
    PLL_init();       // Set system clock to 80 MHz
    SysTick_init();   // SysTick timer
    GPIO_Init();      // Ports for LCD & Keypad
    LCD_Init();       // Initialise LCD (4-bit mode)

    // ---- 2) Calculator Logic Initialisation ----
    Calc_Init();

    // Show a prompt
    LCD_Clear();
    LCD_String("Expression:");  
    // We'll show the typed expression on the first line, 
    // result or messages on the second line.

    // We'll use this flag to detect if the user has just seen a result:
    bool calculationDone = false;

    // ---- 3) Main program loop ----
    while (1)
    {
        // Read one keypad key
        char key = Keypad_GetKey();

        // If no key pressed, continue
        if (key == '\0') {
            continue;
        }

        // If the user presses ANY digit/operator after a calculation,
        // we automatically reset the calculator to start a new expression.
        if (calculationDone) {
            // If not 'C' or '='
            // (If user presses '=' again, we might ignore or handle differently)
            if (key != 'C' && key != '=') {
                // Clear expression for new calculation
                Calc_ClearExpression();
                LCD_Clear();
                LCD_String("Expression:");
            }
            // Mark that we are no longer in the "done" state
            calculationDone = false;
        }

        // Now handle possible keys:
        switch (key) {
        case 'C':
            // Clear everything
            Calc_ClearExpression();
            LCD_Clear();
            LCD_String("Expression:");
            break;

        case '=':
        {
            // Evaluate current expression
            long result = Calc_Evaluate();

            // Clear the second line for the result or error
            LCD_SetCursor(1,0);
            LCD_String("                "); // blank out second line
            LCD_SetCursor(1,0);

            if (Calc_HadError()) {
                // Indicate an error
                LCD_String("Error!");
            } else {
                // Display result
                char buf[16];
                sprintf(buf, "Result=%ld", result);
                LCD_String(buf);
            }

            // After showing the result, set calculationDone = true
            // so that if the user types a new digit/operator,
            // we reset for a new expression.
            calculationDone = true;
            break;
        }

        default:
        {
            // It's likely a digit or +, -, *, /
            // Attempt to add it to the calc buffer
            if (Calc_AddChar(key) < 0) {
                // Buffer full or error
                LCD_SetCursor(1,0);
                LCD_String("Buffer Full!");
            } else {
                // Successfully added character -> refresh LCD line 0 
                // to show the entire expression
                const char* expr = Calc_GetExpression();

                // Clear line 0 before rewriting
                LCD_SetCursor(0,0);
                LCD_String("                "); // 16 spaces for a 16x2 display
                LCD_SetCursor(0,0);
                LCD_String(expr);
            }
            break;
        }
        } // end switch
    }

    // Should never reach here
    return 0;
}
