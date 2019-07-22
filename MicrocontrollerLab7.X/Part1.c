/*
 * File:   Part1.c
 * Author: dominic.klusek
 *
 */
// PIC18F452 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = HS         // Oscillator Selection bits (HS oscillator)
#pragma config OSCS = OFF       // Oscillator System Clock Switch Enable bit (Oscillator system clock switch option is disabled (main oscillator is source))

// CONFIG2L
#pragma config PWRT = ON        // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOR = ON         // Brown-out Reset Enable bit (Brown-out Reset enabled)
#pragma config BORV = 42        // Brown-out Reset Voltage bits (VBOR set to 4.2V)

// CONFIG2H
#pragma config WDT = OFF         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 128      // Watchdog Timer Postscale Select bits (1:128)

// CONFIG3H
#pragma config CCP2MUX = ON     // CCP2 Mux bit (CCP2 input/output is multiplexed with RC1)

// CONFIG4L
#pragma config STVR = ON        // Stack Full/Underflow Reset Enable bit (Stack Full/Underflow will cause RESET)
#pragma config LVP = ON         // Low Voltage ICSP Enable bit (Low Voltage ICSP enabled)

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000200-001FFFh) not code protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot Block (000000-0001FFh) not code protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000200-001FFFh) not write protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0001FFh) not write protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000200-001FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from Table Reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from Table Reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0001FFh) not protected from Table Reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#include "LCD362.h"
#include "keypadHelper.h"
#include <stdlib.h>

#define _XTAL_FREQ 10000000

void sendPattern(unsigned char pattern, unsigned char pattern2);
void initialize();

void main(void) {
    initialize(); //initialize the microcontroller
    __delay_ms(10);
    initLCD(); //initialize the LCD
    
    for (;;) {
        unsigned char pressed;
        unsigned char number1 = 0xFF;
        unsigned char number2 = 0xFF;
        unsigned char op = 0xFF; 
        
        setCursor(0x80);
        writeString("MY CALCU");
        __delay_ms(2000);
        
        clearLCD(); //clear the LCD of previous message
        
        setCursor(0x80); //set cursor
        writeString("No1: "); //write Placeholder text
        
        for(;;){
            pressed = getKeyPad(); //get key position from keypad
            
            pressed = getMappedKey(pressed); //get mapping of key
            
            //if the value of pressed is a number, store value into number1
            if(pressed >= 0 && pressed < 10){
                if(number1 != 0xFF){
                    number1 = (number1 * 10) + pressed;
                } else{
                    number1 = pressed;
                }
                //change displayed number on LCD
                setCursor(0x80);
                unsigned char buf[8];
                sprintf(buf, "No1: %i", number1);
                writeString(buf);
            } else if(pressed == 'E' && number1 != 0xFF){
                break;
            }
            //check if ENTER(D) was pressed
        }
        
        setCursor(0xC0); //set cursor
        writeString("No2: "); //write Placeholder text
        
        for(;;){
            pressed = getKeyPad(); //get key position from keypad
            
            pressed = getMappedKey(pressed); //get mapping of key
            
            //if the value of pressed is a number, store value into number1
            if(pressed >= 0 && pressed < 10){
                if(number2 != 0xFF){
                    number2 = (number2 * 10) + pressed;
                } else{
                    number2 = pressed;
                }
                //change displayed number on LCD
                setCursor(0xC0);
                unsigned char buf[8];
                sprintf(buf, "No2: %i", number2);
                writeString(buf);
            } else if(pressed == 'E' && number2 != 0xFF){
                break;
            }
            //check if ENTER(D) was pressed
        } 
        
        clearLCD(); //clear LCD of previous messages
        setCursor(0x80); //set cursor
        unsigned char buf[8];
        sprintf(buf, "Op:", number1, number2);
        writeString(buf); //write Placeholder text
        
        for(;;){
            pressed = getKeyPad(); //get key position from keypad
            
            pressed = getMappedKey(pressed); //get mapping of key
            
            //if the value of pressed is a number, store value into number1
            if(pressed == '+' || pressed == '-' || pressed == '/' || pressed == '*'){
                op = pressed;
                break;
            }
            //check if ENTER(D) was pressed
        }
        
        clearLCD(); //clear LCD of previous messages
        
        switch(op){
            case '+':
                setCursor(0x80);
                writeString("Res=");
                setCursor(0xC0);
                writeInteger((int)(number1+number2));
                break;
            case '-':
                setCursor(0x80);
                writeString("Res=");
                setCursor(0xC0);
                writeInteger((int)(number1-number2));
                break;
            case '/':
                setCursor(0x80);
                writeString("Res=");
                setCursor(0xC0);
                writeInteger((int)(number1/number2));
                break;
            case '*':
                setCursor(0x80);
                writeString("Res=");
                setCursor(0xC0);
                writeInteger((int)(number1*number2));
                break;
            default:
                break;
        }
        
        __delay_ms(5000);
        clearLCD();
    }
}

/** Add or remove the necessary initializations here.
 */
void initialize() {
    TRISC = 0x00;
    TRISB = 0x0F;
    LATC = 0xFF;
    PORTB = 0;
    
    TRISD = 0; //set Port D to be output, so that they can be read by LCD
    TRISEbits.TRISE0 = 0; //set first two bits of Port E to be outputs
    TRISEbits.TRISE1 = 0;
}
