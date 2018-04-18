#ifndef SPI__H__
#define SPI__H__

#include <xc.h>

#define CS LATAbits.LATA0 

void setVoltage(char channel, char voltage);
void initSPI1();
char SPI_IO(char write);


#endif