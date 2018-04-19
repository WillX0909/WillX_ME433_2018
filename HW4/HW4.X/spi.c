#include "spi.h"
#include <math.h>

void setVoltage(char channel, char voltage)
{   
    unsigned char a;
    unsigned char b;
    
    CS = 0; 
    a = voltage << 4;
    b = ((channel << 7)|(0b111 << 4))|(voltage >> 4);
    SPI_IO(a);
    SPI_IO(b);
    CS = 1; 
}
void initSPI1()
{
    RPA1Rbits.RPA1R = 0b0011;       // SET A1 as SDO1
    RPA0Rbits.RPA0R = 0b0011;       // SET A0 as SS1
    CS = 1;
    SPI1CON = 0;                    // Turn off SPI and reset
    SPI1BUF;                        // Clear buffer
    SPI1BRG = 0x1;                   // Set baud rate to 100kHz
    
    SPI1STATbits.SPIROV = 0;        // Clear overflow bit
    SPI1CONbits.CKE = 1;            // Data changes when clock goes from high to low (since CKP is 0)
    SPI1CONbits.CKP = 1;
    SPI1CONbits.MSTEN = 1;          
    SPI1CONbits.ON = 1;             // Turn on SPI 1   
}

char SPI_IO(char write)
{
    SPI1BUF = write;
    while(!SPI1STATbits.SPIRBF) {   // Wait to receive data
        ;
    }
    return SPI1BUF;
}