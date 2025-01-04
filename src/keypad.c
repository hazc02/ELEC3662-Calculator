#include "keypad.h"
#include "gpio.h"
#include "clock.h"
#include <stdbool.h>

static bool shiftState = false;

static const char normalMap[4][4] = {
    {'1','2','3','+'},
    {'4','5','6','-'},
    {'7','8','9','*'},
    {'S','0','.', '='}
};

static const char shiftedMap[4][4] = {
    {'^','?','?','/'},
    {'s','c','t','C'},
    {'?','?','?','?'},
    {'S','?','?','?'}
};

void Keypad_Init(void)
{
    // If already configured in GPIO_Init, do nothing here.
}

char Keypad_GetKey(void)
{
    unsigned char col, row;
    char c = '\0';

    for(col=0; col<4; col++){
        // Drive all columns high
        GPIO_PORTD_DATA_R |= KEYPAD_COL_MASK;
        // Pull this column low
        GPIO_PORTD_DATA_R &= ~(1<<col);

        delay_us(2);

        unsigned char rowData= GPIO_PORTE_DATA_R & KEYPAD_ROW_MASK;
        for(row=0; row<4; row++){
            if(!(rowData & (1<<row))){
                // Debounce
                delay_ms(20);
                rowData= GPIO_PORTE_DATA_R & KEYPAD_ROW_MASK;
                if(!(rowData & (1<<row))){
                    // Wait release
                    while(!(GPIO_PORTE_DATA_R & (1<<row))){ }

                    c= shiftState ? shiftedMap[row][col] : normalMap[row][col];
                    if(c=='S'){
                        shiftState= !shiftState;
                        return '\0';
                    }
                    return c;
                }
            }
        }
    }
    return '\0';
}
