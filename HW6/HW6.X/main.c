#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "ST7735.h"
#include <stdio.h>

#define BGCOLOR BLACK 
#define TEXTCOLOR GREEN
#define BARCOLOR RED
#define N 20

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

void displayChar(char x, char y, int color, char ch){
    int i, j;
    
    for(i = 0; i < 5; i++){
        int k = ASCII[ch - 0x20][i];
        for(j = 0; j < 8; j++){
            if(x > 128 || y > 160){ // error check that x and y are not out of bounds
                break;
            }else if(k >> j & 1){
                LCD_drawPixel(x+i, y+j, color);
            }else{
                LCD_drawPixel(x+i, y+j, BGCOLOR);
            }
        }
    }
}

void displayStr(char* msg, char x, char y){
    int i = 0;
    while(msg[i] != 0){
        displayChar(x+(i*6), y, TEXTCOLOR, msg[i]);
        i++;
    }
}

void drawBar(char x, char y, char z, char ch, char len, int colora, int colorb){
    int i, j;
    for(i = 0; i < ch; i++){
        if(x>128||y >160){
            break;
        }
        for(j = 0; j < z; j++){
            LCD_drawPixel(x, y+i, colora);
        }
        x++;
    }
    
    for(i = ch; i < len; i++){
        if(x>128||y >160){
            break;
        }
        for(j = 0; j < z; j++){
            LCD_drawPixel(x, y+i, colorb);
        }
        x++;
    }
}

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here

    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 1;
    TRISBbits.TRISB4 = 1;
    
    SPI1_init();
    LCD_init(); 
    LCD_clearScreen(BGCOLOR);
    
    __builtin_enable_interrupts();
    
    char message[N];
    int i;
    
    while(1){
        for(i = 0; i < 100; i++){
            sprintf(message, "Hello World %d ", i);
            displayStr(message,28,32);
            drawBar(28,50,1,i,100,BARCOLOR,BGCOLOR); 
        }
    }
    
}