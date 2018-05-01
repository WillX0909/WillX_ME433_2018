/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"

#define SLAVE_ADDR 0b01101011
#define CTRL1_XL   0b00010000
#define CTRL2_G    0b00010001 
#define CTRL3_C    0b00010010
#define WHO_AM_I   0b00001111
#define OUT_TEMP_L 0b00100000

#define BGCOLOR BLACK 
#define TEXTCOLOR GREEN
#define BARCOLOR RED
#define MAXBARLEN 50

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

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
    i2c_master_send(WHO_AM_I); 
    i2c_master_restart(); 
    i2c_master_send(SLAVE_ADDR << 1 | 1); 
    char ch = i2c_master_recv(); 
    i2c_master_ack(1); 
    i2c_master_stop(); 
    return ch;
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
            break;
        }else{
            i2c_master_ack(0); // keep on reading
        }
    }
    i2c_master_stop();
}

void APP_Initialize ( void ){
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 1;
    TRISBbits.TRISB4 = 1;
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
    i2c_master_setup();
    LCD_init(); 
    initIMU(); 
    LCD_clearScreen(BGCOLOR);
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
        
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            char message[100];
            unsigned char raw[100];
            signed short trans[100];
 
            int i;
            while(1){
        
                        //sprintf(message, " %d ", WhoAmI());
                        //displayStr(message, 10, 100);
                I2C_read_multiple(SLAVE_ADDR, OUT_TEMP_L, raw, 14);
                i = 0;      
                for (i = 0; i < 7; i++) {
                    trans[i] = ((raw[2 * i + 1] << 8) | (raw[2 * i]));
                }
        
                signed short ax = trans[4]*0.003;
                signed short ay = (trans[5])*0.003;
        
                sprintf(message, "ACCELx: %hi", trans[4]);
                displayStr(message, 10, 20);
                sprintf(message, "ACCELy: %hi", trans[5]);
                displayStr(message, 10, 30);
        
                drawXBar(64, 80, 3, ax, MAXBARLEN,BARCOLOR, BGCOLOR);
                drawYBar(64, 80, 3, ay, MAXBARLEN,BARCOLOR, BGCOLOR);
                  
                while (_CP0_GET_COUNT() <= 1200000) {
                    ; //20 Hz
                }
                LATAbits.LATA4 = !LATAbits.LATA4;
            }
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
