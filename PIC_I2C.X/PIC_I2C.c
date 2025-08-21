#include <xc.h>
#include <stdint.h>
#include <stdio.h>

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
    while (SSPIF == 0);         // Wait for Stop condition to complete
    SSPIF = 0;                  // Clear the SSPIF interrupt flag
}

void i2c_rstart(void)
{
    RSEN = 1;                   // Initiate Repeated Start condition
    while (SSPIF == 0);         // Wait for Repeated Start condition to complete
    SSPIF = 0;                  // Clear the SSPIF interrupt flag
}

void i2c_ack(void)
{
    ACKDT = 0;                  // Clear ACK data bit (ACK)
    ACKEN = 1;                  // Initiate ACK sequence
    while (SSPIF == 0);         // Wait for ACK sequence to complete
    SSPIF = 0;                  // Clear the SSPIF interrupt flag
}

void i2c_noack(void)
{
    ACKDT = 1;                  // Set ACK data bit (NACK)
    ACKEN = 1;                  // Initiate NACK sequence
    while (SSPIF == 0);         // Wait for NACK sequence to complete
    SSPIF = 0;                  // Clear the SSPIF interrupt flag
}

int i2c_send(char data)
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
    uart_init();                // Initialize UART communication
    i2c_init();                 // Initialize I2C communication
   
    i2c_start();                // Start I2C communication

    // Send device address and check for ACK
    while (i2c_send(0xD0) == 1)
    {
        i2c_start();            // If no ACK received, retry
    }
   
    // Send initial time values to RTC
    i2c_send(0x00);             // Seconds
    i2c_send(0x00);             // Minutes
    i2c_send(0x00);             // Hours
    i2c_send(0x00);             // Day of the week
    i2c_stop();                 // Stop I2C communication

    while (1)
    {
        i2c_start();            // Start I2C communication

        // Send device address and check for ACK
        while (i2c_send(0xD0) == 1)
        {
            i2c_start();        // If no ACK received, retry
        }
        
        i2c_send(0x00);         // Send address to start reading from (seconds)
        i2c_rstart();           // Repeated Start condition

        i2c_send(0xD1);         // Send device address for reading (0xD1 for read mode)
        s = i2c_read();         // Read seconds and store
        i2c_ack();              // Send ACK
        m = i2c_read();         // Read minutes and store
        i2c_ack();              // Send ACK
        h = i2c_read();         // Read hours and store
        i2c_noack();            // Send NACK to stop reading
        i2c_stop();             // Stop I2C communication
        
        // Format time and send via UART
        sprintf(s1, "Time=%02x:%02x:%02x \r\n", h, m, s);
        uart_string_send(s1);
        
        __delay_ms(1000);       // Delay for approximately 1 second
    }
    return;
}
