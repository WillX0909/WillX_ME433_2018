#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<math.h>
#include"spi.h"


// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module


int main() {
    unsigned int sine_wave[100];
    unsigned int ramp_wave[200];
    int i,j;
    __builtin_disable_interrupts();

    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA0 = 0;  // A0 OUTPUT
    TRISAbits.TRISA1 = 1;  // A1 INPUT
    TRISAbits.TRISA4 = 0;  // A4 OUTPUT
    TRISBbits.TRISB4 = 1;  // B4 INPUT
    LATAbits.LATA4 = 1;    // A4 = 1
    
    initSPI1();
    
    for(i=0; i<100; i++){
        sine_wave[i] = (255.0/2.0) + (255.0/2.0)*sin(2*3.14*(i/100.0));
    }
   
    for(j=0; j<200; j++){ 
        ramp_wave[j] = (j/200.0)*255.0;
    }

    __builtin_enable_interrupts();

    _CP0_SET_COUNT(0);
    i = 0;
    while(1) {
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 10000) {}
        setVoltage(0, sine_wave[100]); 
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 10000) {}
        setVoltage(1, ramp_wave[200]);
        i++;
        if (i==200){
         i = 0;   
        }
    }
    return 0;
}