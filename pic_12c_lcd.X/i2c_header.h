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