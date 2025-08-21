#include <xc.h>

// Configuration bits
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage In-Circuit Serial Programming Enable bit (ICSP disabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)
#define _XTAL_FREQ 4000000      // Oscillator frequency for delay functions

void main(void) 
{
    // Initialize SPI slave mode
    TXSTA = 0x25;     // Transmit Control Register: TXEN = 1, SYNC = 0, BRGH = 0
    RCSTA = 0x90;     // Receive Control Register: SPEN = 1, CREN = 1
    SPBRG = 25;       // Baud Rate Generator: SPBRG = ((Fosc/4) / BaudRate) - 1 = 25 (for 9600 baud at 4 MHz)

    TRISC3 = 1;       // Set RC3 (SCK) as input (Slave Clock)
    TRISC4 = 1;       // Set RC4 (SDI) as input (Serial Data In)
    TRISC5 = 0;       // Set RC5 (SDO) as output (Serial Data Out)

    SSPSTAT = 0x00;   // SPI Status Register: SMP = 0, CKE = 0
    SSPCON = 0x25;    // SPI Control Register: SSPEN = 1, CKP = 0, SSPM3:SSPM0 = 0101 (SPI Slave mode, clock = SCK pin)
    TRISB=0x00;
    PORTB=0x00;
    while (1) {
        // Wait for SPI interrupt flag
        while (!SSPIF);
        PORTB=SSPBUF;;
        // Read data from SPI buffer
        //char receivedData = SSPBUF;
        TXREG = SSPBUF;

        // Wait for transmit shift register to be empty
        while (!TRMT);
        SSPIF = 0;
    } 
}
