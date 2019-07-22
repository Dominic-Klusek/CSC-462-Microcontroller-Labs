/*
 * File:   Part4.c
 * Author: dominic.klusek
 *
 * Created on October 31, 2018, 8:52 PM
 */
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


#define _XTAL_FREQ 10000000

#define Increment PORTBbits.RB3
#define Decrement PORTBbits.RB4

#define Pressed 1
#include "LCD362.h"
#include <string.h>

void initialize();

void main(void) {
    int count = 0;
    float count2 = 0.0;
    initialize();
    
    __delay_ms(10);
    initLCD(); // initialize the LCD screen
    
    __delay_ms(10);
    clearLCD(); //clear the LCD
    
    char name[] = "Samantha Rivera"; //array holding name
    
    
    for(;;){
        setCursor(0x80);//set cursor to first column of first row
        writeString(name);//write string to LCD
        //shift the name back off the screen
        for(int i = 0; i < strlen(name); i++){
            shiftLCDLeft();
            __delay_ms(500);
        }
        //shift the name back onto the screen
        for(int i = 0; i < strlen(name); i++){
            shiftLCDRight();
            __delay_ms(500);
        }
        
        __delay_ms(5000);//five second delay
        clearLCD();
        for(int i = 0; i < 100; i++){
            setCursor(0x80);
            writeInteger(count);
            setCursor(0xc0);
            writeFloat(count2);
            count++;
            count2+=0.5;
            if(count2 > 5.0){
                count2 = 0.0;
            }
            __delay_ms(200);
        }
        count = 0;
        count2 = 0;
        clearLCD();
    }
    return;
}

void initialize() {
    TRISBbits.TRISB3 = 1;
    TRISBbits.TRISB4 = 1;
    TRISD = 0;
    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
}