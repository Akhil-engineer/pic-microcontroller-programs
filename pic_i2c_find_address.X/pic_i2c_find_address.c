#include <xc.h>
#include <stdint.h>
#include <stdio.h>

// Configuration bits settings
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Disable
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits
#pragma config CP = OFF         // Flash Program Memory Code Protection bit

#define _XTAL_FREQ 4000000      // Crystal frequency for delay calculation

char s1[20];                    // Array to store formatted string

// Function prototypes
void uart_init(){
    TRISCbits.TRISC6 = 0;       // RC6/TX pin set as output
    TRISCbits.TRISC7 = 1;       // RC7/RX pin set as input
    TXSTA = 0x24;               // TXSTA: TX enable, asynchronous mode, high baud rate select
    RCSTA = 0x90;               // RCSTA: Enable serial port, 8-bit continuous receive mode
    SPBRG = 25;                 // Baud rate generator setting (for 9600 baud rate at 4 MHz oscillator)
}

void uart_send(unsigned char data){
    while (!TXIF);              // Wait until transmit buffer is empty
    TXREG = data;               // Load data into transmit register
}

void uart_string_send(unsigned char *data){
    while (*data != '\0'){
        uart_send(*data);
        data++;
    }
}

void i2c_init(){
    SSPSTAT = 0x80;             // SMP = 1, CKE = 0
    SSPCON = 0x28;              // SSPEN = 1, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
    SSPADD = 9;                 // Baud rate generator for 100 kHz I2C clock at 4 MHz FOSC
}

void i2c_start(void)
{
    SEN = 1;                    // Initiate Start condition
    while (SEN);                // Wait for Start condition to complete
    SSPIF = 0;                  // Clear the SSPIF interrupt flag
}

void i2c_stop(void)
{
    PEN = 1;                    // Initiate Stop condition
    while (PEN);                // Wait for Stop condition to complete
    SSPIF = 0;                  // Clear the SSPIF interrupt flag
}

int i2c_send(unsigned char data)
{
    SSPBUF = data;              // Load data into SSPBUF for transmission
    while (SSPIF == 0);         // Wait for transmission to complete
    SSPIF = 0;                  // Clear the SSPIF interrupt flag
    return ACKSTAT;             // Return ACK/NACK status
}

char i2c_read(void)
{
    RCEN = 1;                   // Enable I2C receive mode
    while (SSPIF == 0);         // Wait for receive buffer to be full
    SSPIF = 0;                  // Clear the SSPIF interrupt flag
    return SSPBUF;              // Return received data from SSPBUF
}

void main(void) {
    char f=0;
    unsigned char address;
    uart_init();                // Initialize UART communication
    i2c_init();                 // Initialize I2C communication
    uart_string_send("**********I2C SCAN BEGINS************\r\n");
    for (address = 0x08; address <= 0x77; address++) {
        i2c_start();            // Start I2C communication

        // Send device address and check for ACK
        if (i2c_send(address << 1) == 0) {
            // ACK received, device present at this address
            sprintf(s1, "Device found at address: %x\r\n", address);
            uart_string_send(s1);
            f=1;
        }

        i2c_stop();             // Stop I2C communication

        __delay_ms(1);        // Delay between address checks
    }
    if(!f){
        uart_string_send("no device found");
    }
    while (1) {
        // Main program loop
    }

    return;
}
