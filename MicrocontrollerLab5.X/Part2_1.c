/*
 * File:   Part2.c
 * Author: dominic.klusek
 *
 * Created on October 24, 2018, 8:10 PM
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
#include <stdlib.h>

#define _XTAL_FREQ 10000000

#define Increment PORTBbits.RB3
#define Decrement PORTBbits.RB4

#define Pressed 1

unsigned char number(int lim);
void sendPattern(unsigned char pattern, unsigned char pattern2);
unsigned char rotateLeft(unsigned char pattern, int no);
void resetPins();
void initialize();

void main(void) {
    unsigned char isHeld = 0;
    signed char count = 00;
    unsigned char SEGMENT[] = {64, 121, 36, 48, 25, 18, 2, 120, 0, 16};
    // Define your dice array here
    initialize();
    for (;;) {
        if((Increment == Pressed) && (isHeld == 0)){
            count++;
            
            if(count >= 100)
                count = 0;
        } else if((Decrement == Pressed) && (isHeld == 0)){
            count--;
            
            if(count < 0)
                count = 0;
        }
        sendPattern(SEGMENT[count % 10], SEGMENT[(count - (count % 10)) / 10]); //call function to send pattern to LEDs
        
        if((Increment == Pressed) || (Decrement == Pressed))
            isHeld = 1;
        else
            isHeld = 0;
    }
}

/** Send pattern to turn off some of segments in 7-segment display
 */
void sendPattern(unsigned char pattern, unsigned char pattern2){
    PORTC = pattern & 63; //send pattern1 to digit 1
    PORTB = (pattern >> 6); //must bitwise-OR with PORTB to retain LEDs that are on
    
    PORTBbits.RB1 = 0; //turn on digit1
    PORTBbits.RB2 = 1;//turn off digit2
    
    __delay_ms(10); // Delay for 10 milliseconds
    
    PORTBbits.RB1 = 1; //turn off digit1
    
    PORTC = pattern2 & 63; //send pattern1 to digit 2
    PORTB = pattern2 >> 6; //must bitwise-OR with PORTB to retain LEDs that are on
    
    PORTBbits.RB1 = 1; //turn off digit1
    PORTBbits.RB2 = 0; //turn on digit2
    
    __delay_ms(10); // Delay for 10 milliseconds
    
    PORTBbits.RB2 = 1;//turn off digit2
}

void resetPins(){
    //PORTBbits.RB1 = 1; //turn off LEDs
    PORTBbits.RB2 = 1;
    
    PORTBbits.RB0 = 0; //clear ports
    PORTC = 0x00;
}

/** Add or remove the necessary initializations here.
 */
void initialize() {
    TRISC = 0;
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 1;
    TRISBbits.TRISB4 = 1;
    PORTC = 0x00;
    PORTB = 0x00;
    PORTBbits.RB1 = 1; //turn off digit1
    PORTBbits.RB2 = 1;//turn off digit2
}
