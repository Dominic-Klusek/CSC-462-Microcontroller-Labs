/*
 * File:   LED_dice.c
 * Author: ENS 362
 *
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
#include <stdlib.h>

#define _XTAL_FREQ 10000000

#define Switch PORTBbits.RB1
#define Pressed 1

unsigned char number(int lim);
void sendPattern(unsigned char pattern);
unsigned char rotateLeft(unsigned char pattern, int no);
void resetPins();
void initialize();

void main(void) {
    unsigned char j = 0;
    unsigned char pattern[] = {0,64,18,82,45,109,63};
    // Define your dice array here
    initialize();
    for (;;) {
        if (Switch == Pressed) {
            // You could also use: j = rand() % 6 + 1
             j = number(6);
             sendPattern(pattern[j]);
             // Get your dice pattern value from your DICE array and send
             // it to ports.
        }
    }
}

/** Generate random number between 1 and lim
 */
unsigned char number(int lim) {
    unsigned char result;
    static unsigned int x = 1;
    x = (x * 32719 + 3) % 32749;
    result = ((x % lim) + 1);
    return result;
}

/** Send pattern to light LED dice
 */
void sendPattern(unsigned char pattern) {
    // Send pattern to appropriate port pins
    PORTC = pattern & 63;
    PORTB = (((pattern & 64) >> 6) | PORTB);
    __delay_ms(3000); // Delay for 3 seconds
    // Reset pins (i.e. set values to zero to turn off the LEDs)
    resetPins(); 
}

/** Rotate pattern to the left 'no' number of bits
 */
unsigned char rotateLeft(unsigned char pattern, int no) {
    return (((pattern << no) & 0xFF) | (pattern >> (8-no)));
}

void resetPins(){
    PORTBbits.RB0 = 0;
    PORTC = 0x00;
}

/** Add or remove the necessary initializations here.
 */
void initialize() {
    TRISC = 0;
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 1;
    PORTC = 0x00;
    PORTB = 0x00;
}