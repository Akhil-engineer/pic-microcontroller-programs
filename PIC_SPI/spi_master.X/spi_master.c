#include <xc.h>  // Include the XC8 compiler header file for PIC microcontrollers

// Configuration settings for the PIC microcontroller
#pragma config FOSC = XT        // XT oscillator
#pragma config WDTE = OFF       // Watchdog Timer disabled
#pragma config PWRTE = OFF      // Power-up Timer disabled
#pragma config BOREN = OFF      // Brown-out Reset disabled
#pragma config LVP = OFF        // Low-Voltage Programming disabled
#pragma config CPD = OFF        // Data EEPROM Code Protection off
#pragma config WRT = OFF        // Flash Program Memory Write Protection off
#pragma config CP = OFF         // Flash Program Memory Code Protection off

#define _XTAL_FREQ 4000000     // Crystal frequency 4 MHz

void main(void) 
{
    TRISC3 = 0;  // RC3 as output (SPI clock)
    TRISC4 = 1;  // RC4 as input (SPI data in)
    TRISC5 = 0;  // RC5 as output (SPI data out)
    
    TXSTA = 0x24;  // Configure USART transmitter
    RCSTA = 0x90;  // Configure USART receiver
    SPBRG = 25;    // Baud rate set to 25 for USART communication

    SSPSTAT = 0x00;  // Clear SPI status register
    SSPCON = 0x20;   // Configure SPI control register for master mode
    TRISB=0X00;
    PORTB=0X00;
    while(1)
    {
       
            SSPBUF ='a';     // Load 'A' into SPI buffer for transmission
            while(SSPIF == 0);  // Wait for SPI transfer complete
            SSPIF = 0;        // Clear SPI interrupt flag
            __delay_ms(1000);
        
    }
}
