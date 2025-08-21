#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include"i2c_header.h"
// Configuration bits settings
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits
#pragma config CP = OFF         // Flash Program Memory Code Protection bit

#define _XTAL_FREQ 4000000      // Crystal frequency for delay calculation

// Global variables
int s, m, h;                    // Variables to store time
char s1[40];                    // Array to store formatted time string
unsigned char PORTD=0x00;

/****************************************************************************************/

void lcd_command(unsigned char command) {
    
    PORTD=1<<3;
    PORTD=(PORTD&0x0F)|(command&0xf0);
    i2c_send(PORTD); 
    PORTD|=(1<<2);
    i2c_send(PORTD); 
    __delay_ms(10); 
     PORTD&=~(1<<2);
    i2c_send(PORTD); 
    
    PORTD=(PORTD&0x0F)|(command<<4);
    i2c_send(PORTD); 
     
    PORTD|=(1<<2);
    i2c_send(PORTD); 
    __delay_ms(10); 
     PORTD&=~(1<<2);
    i2c_send(PORTD); 
    
    
}

void lcd_data(unsigned char data) {
    //RS = 1; // Data mode
    //RW = 0; // Write mode
    //lcd_send_byte(data);
    
    
    
    PORTD=(1<<0)|(1<<3);
    PORTD=(PORTD&0x0F)|(data&0xf0);
    i2c_send(PORTD); 
      
    PORTD|=(1<<2);
    i2c_send(PORTD); 
    __delay_ms(10); 
     PORTD&=~(1<<2);
    i2c_send(PORTD); 
    
      
    
     PORTD=(PORTD&0x0F)|(data<<4);
    i2c_send(PORTD); 
     
    PORTD|=(1<<2);
    i2c_send(PORTD); 
    __delay_ms(10); 
     PORTD&=~(1<<2);
    i2c_send(PORTD); 
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

void lcd_backlight_on(){
    PORTD|=(1<<3);
}


void lcd_backlight_off(){
    PORTD&=~(1<<3);
}
/****************************************************************************************/
void main(void) {
                 // Initialize UART communication
    i2c_init();                 // Initialize I2C communication
   
    i2c_start();                // Start I2C communication

    // Send device address and check for ACK
    while (i2c_send(0x27<<1) == 1)
    {
        i2c_start();            // If no ACK received, retry
    }
   lcd_init();
        lcd_set_cursor(0, 0); // Set cursor to the beginning of the first row
    lcd_write_string("AKHIL");
    
    
    while (1)
    {
        
    }
    
    
    return;
}
