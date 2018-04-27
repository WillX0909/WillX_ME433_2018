#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "i2c_master_noint.h"
#include <stdio.h>
#include "ST7735.h"

#define SLAVE_ADDR 0x6B
#define CTRL1_XL   0x10
#define CTRL2_G    0x11 
#define CTRL3_C    0x12
#define WHO_AM_I   0x0F
#define OUT_TEMP_L 0x20
#define OUTX_L_XL  0x28

#define BGCOLOR BLACK 
#define TEXTCOLOR GREEN
#define BARCOLOR RED
#define MAXBARLEN 50

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
#pragma config OSCIOFNC = OFF // free up secondary osc pins ??
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz (input clock is 8MHz crystal)
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

void initIMU(void){
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1 | 0);
    i2c_master_send(CTRL1_XL);
    i2c_master_send(0b10000010);
    i2c_master_stop();
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR <<1 | 0);
    i2c_master_send(CTRL2_G);
    i2c_master_send(0b10001000);
    i2c_master_stop();
    
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR <<1 | 0);
    i2c_master_send(CTRL3_C);
    i2c_master_send(0b00000100);
    i2c_master_stop();
    
}

void displayChar(short x, short y, short color, char ch){
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

void drawXBar(short x, short y, short width, short len, short maxlen, short colora, short colorb){
    int i, j;
    if(len >= 0){
        for(i = 0; i < len; i++){
            for(j = 0; j < width; j++){
                LCD_drawPixel(x, y + j, colora);
            }
            x++;
        }
        for(i = len; i < maxlen; i++){
            for(j = 0; j < width; j++){
                LCD_drawPixel(x, y + j, colorb);
            }
            x++;
        }
    }else{
        for(i = len; i < 0; i++){
            for (j = 0; j < width; j++) {
                LCD_drawPixel(x, y + j, colora); 
            }
            x--;
        }
        for(i = -maxlen; i < len; i++) {
            for(j = 0; j < width; j++) {
                LCD_drawPixel(x, y + j, colorb); 
            }
            x--;
        }
    }
}

void drawYBar(short x, short y, short width, short len, short maxlen, short colora, short colorb){
    int i, j;
    if(len >= 0){
        for(i = 0; i < len; i++){
            for(j = 0; j < width; j++){
                LCD_drawPixel(x + j, y, colora);
            }
            y++;
        }
        for(i = len; i < maxlen; i++){
            for(j = 0; j < width; j++){
                LCD_drawPixel(x + j, y, colorb);
            }
            y++;
        }
    }else{
        for(i = len; i < 0; i++){
            for (j = 0; j < width; j++) {
                LCD_drawPixel(x + j, y, colora); 
            }
            y--;
        }
        for(i = -maxlen; i < len; i++) {
            for(j = 0; j < width; j++) {
                LCD_drawPixel(x + j, y, colorb); 
            }
            y--;
        }
    }
}

char WhoAmI(void){
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1 | 0); 
    i2c_master_send(WHO_AM_I_ADDR); 
    i2c_master_restart(); 
    i2c_master_send(SLAVE_ADDR << 1 | 1); 
    char ch = i2c_master_recv(); 
    i2c_master_ack(1); 
    i2c_master_stop(); 
    return r;
}

void I2C_read_multiple(unsigned char address, unsigned char reg, char* msg, int len){
    i2c_master_start();
    i2c_master_send(address << 1 | 0);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send(address << 1 | 1);
    
    int i;
    for(i = 0; i < len; i++){
        msg[i] = i2c_master_recv();
        if(i == len - 1){
            i2c_master_ack(1);
        }else{
            i2c_master_ack(0); // keep on reading
        }
    }
    i2c_master_stop();
}

int main(void) {
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
    TRISBbits.TRISB4 = 1; //sets RB4 (pin 11, Push Button) as input
    TRISAbits.TRISA4 = 0; //sets RA4 (pin 12, Green LED) as output
    LATAbits.LATA4 = 1; //sets Green LED to be high output 
    ANSELBbits.ANSB2 = 0; //turn off analog function on pin 6 of pic32
    ANSELBbits.ANSB3 = 0; //turn off analog function on pin 7 of pic32

    i2c_master_setup();
    LCD_init(); 
    initIMU(); 
    LCD_clearScreen(BGCOLOR);
    __builtin_enable_interrupts();
    
    
    unsigned char message[200];
    short raw[200];
    short trans[200];
    short ax;
    short ay;
    int i;
    
    while(1){
        
        sprintf(message, " %d ", WhoAmI());
        displayStr(message, 10, 10);
        
        I2C_read_multiple(SLAVE_ADDR, OUT_TEMP_L, raw, 14);
        
        for (i = 0; i < 7; i++) {
            trans[i] = ((raw[2 * i + 1] << 8) | (raw[2 * i]));
        }
        
        ax = trans[4] / 500;
        ay = trans[5] / 500;
        
        sprintf(message, "ACCELx: %hi", trans[4]);
        displayStr(message, 10, 10);
        sprintf(message, "ACCELy: %hi", trans[5]);
        displayStr(message, 10, 20);
        
        drawXBar(64, 80, 4, ax, MAXBARLEN,BARCOLOR, BGCOLOR);
        drawYBar(64, 80, 4, ay, MAXBARLEN,BARCOLOR, BGCOLOR);
    }
}