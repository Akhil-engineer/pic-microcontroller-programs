#include <xc.h>
#include <stdint.h>

// PIC16F876A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>




#define _XTAL_FREQ 4000000

#define RS RC0
#define RW RC1
#define EN RC2

void lcd_enable_pulse(void) {
    EN = 1;
    __delay_ms(10); // Adjust delay as per LCD specifications
    EN = 0;
}

void lcd_send_nibble(unsigned char nibble) {
    PORTC = (PORTC & 0x0F) | (nibble & 0xF0);
    lcd_enable_pulse();
}

void lcd_send_byte(unsigned char data) {
    lcd_send_nibble(data & 0xF0);
    lcd_send_nibble((data << 4) & 0xF0);
}

void lcd_command(unsigned char command) {
    RS = 0; // Command mode
    RW = 0; // Write mode
    lcd_send_byte(command);
}

void lcd_data(unsigned char data) {
    RS = 1; // Data mode
    RW = 0; // Write mode
    lcd_send_byte(data);
}

void lcd_init(void) {
    __delay_ms(15); // Power-on delay
    lcd_command(0x02); // Return home
    lcd_command(0x28); // Function set: 4-bit mode, 2 lines, 5x8 font
    lcd_command(0x0C); // Display control: Display ON, Cursor OFF, Blink OFF
    lcd_command(0x06); // Entry mode set: Increment cursor position, no display shift
    lcd_command(0x01); // Clear display
    __delay_ms(3); // Clear display command delay
}

void lcd_set_cursor(unsigned char row, unsigned char col) {
    unsigned char address;

    if (row == 0) {
        address = col;
    } else {
        address = 0x40 + col; // Address for the second row starts from 0x40
    }

    lcd_command(0x80 | address); // Set DDRAM address
}

void lcd_write_string(char *str) {
    while (*str != '\0') {
        lcd_data(*str);
        str++;
    }
}

void main(void) {
    TRISC = 0x00; // Set PORTC as output
    PORTC = 0x00; // Initialize PORTC
    
    lcd_init(); // Initialize LCD
    
    lcd_set_cursor(0, 0); // Set cursor to the beginning of the first row
    lcd_write_string("hello world");
    
    while (1) {
        // Main program loop
    }
}
