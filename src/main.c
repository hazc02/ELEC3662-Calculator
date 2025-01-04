#include <stdint.h>
#include <stdbool.h>
#include <string.h>  // for strlen
#include <stdio.h>   // for sprintf
#include "clock.h"
#include "gpio.h"
#include "lcd.h"
#include "keypad.h"
#include "calc.h"

#define LCD_COLUMNS 16

int main(void)
{
    PLL_init();
    SysTick_init();
    GPIO_Init();
    LCD_Init();
    Keypad_Init();
    Calc_Init();

    LCD_Clear();

    // If user just did '=', next digit => new expression, next operator => continue from last.
    bool justEvaluated=false;

    while(1){
        char key= Keypad_GetKey();
        if(key=='\0'){
            // no press or SHIFT toggling
            continue;
        }

        // If we just evaluated, handle new key
        if(justEvaluated){
            // if digit/trig => new expression
            if( (key>='0' && key<='9') || key=='.' || key=='s' ||key=='c'||key=='t') {
                Calc_ClearExpression();
                LCD_Clear();
            }
            justEvaluated=false;
        }

        // 'C' => clear
        if(key=='C'){
            Calc_ClearExpression();
            LCD_Clear();
            continue;
        }

        // '=' => evaluate
        if(key=='='){
            double answer= Calc_Evaluate();

            // Clear row=1
            LCD_SetCursor(1,0);
            for(int i=0;i<LCD_COLUMNS;i++){
                LCD_Data(' ');
            }
            LCD_SetCursor(1,0);

            if(Calc_HadError()){
                LCD_String("Error!");
            }
            else{
                // Format up to 3 decimals
                char outBuf[32];
                sprintf(outBuf,"%.3f", answer);
                // strip trailing zeros
                int length= (int)strlen(outBuf);
                while(length>0 && outBuf[length-1]=='0'){
                    length--;
                    outBuf[length]='\0';
                }
                if(length>0 && outBuf[length-1]=='.'){
                    length--;
                    outBuf[length]='\0';
                }

                LCD_String(outBuf);
            }
            justEvaluated=true;
            continue;
        }

        // Attempt to add key to expression
        if(Calc_AddChar(key)<0){
            // buffer full => reset
            Calc_ClearExpression();
            LCD_Clear();
            continue;
        }

        // Display expression on row=1
        const char* expr= Calc_GetExpression();
        LCD_SetCursor(1,0);
        for(int i=0;i<LCD_COLUMNS;i++){
            LCD_Data(' ');
        }
        LCD_SetCursor(1,0);
        LCD_String(expr);
    }

    return 0;
}
