#include "lcd.h"
#include "gpio.h"
#include "clock.h"

// Definitions for control pins on Port A
#define LCD_RS 0x08  // PA3: Register Select (1 for data, 0 for command)
#define LCD_EN 0x04  // PA2: Enable pin

// Definitions for data pins on Port B
#define LCD_DATA_PINS 0x0F  // PB0-PB3 connected to LCD DB4-DB7

// Internal function prototypes
static void LCD_SendNibble(unsigned char nibble, unsigned char isData);
static void LCD_SendByte(unsigned char byte, unsigned char isData);

void LCD_Init(void) {
    // Allow LCD power to stabilise
    delay_ms(20);

    // Ensure control lines are low
    GPIO_PORTA_DATA_R &= ~(LCD_RS | LCD_EN);

    // Initialise LCD in 8-bit mode (hardware default on boot) with three function set commands
    LCD_SendNibble(0x03, 0);
    delay_ms(1);  
    LCD_SendNibble(0x03, 0);
    delay_ms(1);  
    LCD_SendNibble(0x03, 0);
    delay_ms(1);

    // Step 3: Switch to 4-bit mode
    LCD_SendNibble(0x02, 0);
    delay_ms(1);

    // Configure LCD for 2-line display and 5x8 character font
    LCD_Command(0x28);  // Function set: 4-bit mode, 2 lines, 5x8 dots
    delay_ms(1);

    // Turn off the display while configuring
    LCD_Command(0x08);  // Display off, cursor off, blink off
    delay_ms(1);

    // Clear the display
    LCD_Command(0x01);  // Clear display command
    delay_ms(2);        // Longer delay for clear command

    // Set entry mode to increment cursor, no shift
    LCD_Command(0x06); 
    delay_ms(1);

    // Turn on the display with cursor off
    LCD_Command(0x0C);  // Display on, cursor off, blink off
    delay_ms(1);
}

void LCD_Command(unsigned char command) {
    LCD_SendByte(command, 0);  // Send command byte (isData = 0)
}

void LCD_Data(unsigned char data) {
    LCD_SendByte(data, 1);  // Send data byte (isData = 1)
}

void LCD_Clear(void) {
    LCD_Command(0x01);  // Send clear display command
    delay_ms(2);        // Delay >1.52ms for processing
}

void LCD_String(char *str) {
    // Send each character in the string to the LCD
    while (*str) {
        LCD_Data(*str++);
    }
}

void LCD_SetCursor(unsigned char row, unsigned char col) {
    unsigned char address;

    // Calculate the DDRAM address based on the row and column
    switch (row) {
        case 0:
            address = 0x80 + col;  // Line 1 base address
            break;
        case 1:
            address = 0xC0 + col;  // Line 2 base address
            break;
        default:
            address = 0x80 + col;  // Default to Line 1
    }

    LCD_Command(address);  // Send address command to LCD
}

static void LCD_SendByte(unsigned char byte, unsigned char isData) {
    // Send the upper nibble (4 bits)
    LCD_SendNibble(byte >> 4, isData);

    // Send the lower nibble (4 bits)
    LCD_SendNibble(byte & 0x0F, isData);

    // Add delay for processing
    if (byte == 0x01 || byte == 0x02) {
        delay_ms(2);  // Clear or home commands require >1.52ms
    } else {
        delay_us(50);  // Other commands require >37�s
    }
}

static void LCD_SendNibble(unsigned char nibble, unsigned char isData) {
    // Set RS line based on data/command mode
    if (isData) {
        GPIO_PORTA_DATA_R |= LCD_RS;  // RS = 1 for data
    } else {
        GPIO_PORTA_DATA_R &= ~LCD_RS;  // RS = 0 for command
    }

    // Send nibble to data pins
    GPIO_PORTB_DATA_R &= ~LCD_DATA_PINS;         // Clear data pins
    GPIO_PORTB_DATA_R |= (nibble & 0x0F);        // Set new nibble

    // Small delay to meet setup time
    delay_us(1);

    // Pulse the EN line
    GPIO_PORTA_DATA_R |= LCD_EN;   // EN = 1
    delay_us(1);                   // EN high pulse width (>450ns)
    GPIO_PORTA_DATA_R &= ~LCD_EN;  // EN = 0

    // Small delay for hold time
    delay_us(1);

    // Additional delay for processing (>37us)
    delay_us(37);
}
