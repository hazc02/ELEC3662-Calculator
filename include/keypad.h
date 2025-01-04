#ifndef KEYPAD_H
#define KEYPAD_H

/**
 * SHIFT-latching keypad in pure C:
 * Normal: digits + . + basic ops + '='
 * SHIFT:  trig letters, exponent '^', 'C' clear, ignoring '?' 
 */

void Keypad_Init(void);
char Keypad_GetKey(void);

#endif // KEYPAD_H
