#include "keypad.h"
#include "gpio.h"
#include "clock.h"

// Keypad mapping
static const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Definitions for keypad pins
#define KEYPAD_COL_PORT GPIO_PORTD_DATA_R
#define KEYPAD_ROW_PORT GPIO_PORTE_DATA_R

char Keypad_GetKey(void) {
    unsigned char col, row;

    // Iterate through each column
    for (col = 0; col < 4; col++) {
        // Set all columns high
        KEYPAD_COL_PORT |= KEYPAD_COL_MASK;

        // Pull the current column low
        KEYPAD_COL_PORT &= ~(1 << col);

        // Small delay to allow signals to settle
        delay_us(2);

        // Read all rows
        unsigned char row_data = KEYPAD_ROW_PORT & KEYPAD_ROW_MASK;

        // Check each row to see if a key is pressed  
        for (row = 0; row < 4; row++) {
            if (!(row_data & (1 << row))) { // Active low due to pull-up resistors

                delay_ms(20); // Debounce delay

                // Confirm the key is still pressed
                row_data = KEYPAD_ROW_PORT & KEYPAD_ROW_MASK;
                if (!(row_data & (1 << row))) {

                    // Wait until the key is released
                    while (!(KEYPAD_ROW_PORT & (1 << row)));
                    
                    // Return the corresponding character
                    return keymap[row][col];
                }
            }
        }
    }

    // No key pressed
    return '\0';
}
