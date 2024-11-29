#ifndef LCD_H
#define LCD_H

// Function prototypes for LCD operations

/**
 * @brief Initializes the LCD display.
 *        Configures the display for 4-bit mode and sets default parameters.
 */
void LCD_Init(void);

/**
 * @brief Sends a command to the LCD.
 * @param command The command byte to be sent.
 */
void LCD_Command(unsigned char command);

/**
 * @brief Sends a single character to the LCD for display.
 * @param data The character to be displayed.
 */
void LCD_Data(unsigned char data);

/**
 * @brief Clears the LCD display and resets the cursor position.
 */
void LCD_Clear(void);

/**
 * @brief Displays a string of characters on the LCD.
 * @param str Pointer to the null-terminated string to be displayed.
 */
void LCD_String(char *str);

/**
 * @brief Moves the cursor to the specified position on the LCD.
 * @param row The row number (0-based index).
 * @param col The column number (0-based index).
 */
void LCD_SetCursor(unsigned char row, unsigned char col);

#endif // LCD_H
