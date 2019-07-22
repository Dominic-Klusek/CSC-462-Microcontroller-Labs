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
#include "LCD362.h"
#include <stdlib.h>

#define _XTAL_FREQ 10000000

#define Pressed 1

void sendPattern(unsigned char pattern, unsigned char pattern2);
void initialize();
void writeMessageOnLCD();

void __interrupt() ISR(void){
    if(INTCONbits.INT0F == 1){
        setCursor(0xc0); //setCursor to second row on LCD
        
        writeString("INTRRUPT"); //display INTRRUPT on LCD
        
        LATAbits.LATA1 = 1; //turn on the LEDs(which are low current)
        LATAbits.LA2 = 1; 
        LATAbits.LA3 = 1; 
        
        __delay_ms(3000); //delay for 3 seconds
        
        LATAbits.LA1 = 0; //turn off the LEDs(which are low current)
        LATAbits.LA2 = 0; 
        LATAbits.LA3 = 0; 
        
        //clearLCD(); //clear the LCD to remove the INTRRUPT message
        
        INTCONbits.INT0F = 0; //clear flag for INT0
    } 
}

void main(void) {
    signed char count = 00; //variable to hold value of count
    unsigned char SEGMENT[] = {64, 121, 36, 48, 25, 18, 2, 120, 0, 16};
    initialize(); //initialize the microcontroller
    
    __delay_ms(10); //delay to allow LCD processor to turn on
    
    initLCD(); //intialize the LCD
    
    for (;;) {
        
        //when the counter rolls over, we need to clear the LCD to prevent ghosting
        if(count >= 100){
            clearLCD();
            count = 0;
        }
        
        //when we transition from single to double digits need to use different cursor, 
        //and display the count in decimal and binary
        if(count >= 10){
            setCursor(0x86);
            writeInteger(count);
            writeAsBinary(count,0xc0);
        } else{
            setCursor(0x87);
            writeInteger(count);
            writeAsBinary(count,0xc0);
        }
        
        //after the number are written to the LCD, we write to the 7 segment displays
        //we also repeat this multiple times to create a delay of ~1 second
        for(int i = 0; i < 46; i++){
            sendPattern(SEGMENT[count % 10], SEGMENT[(count - (count % 10)) / 10]); //call function to send pattern to LEDs
            __delay_ms(1);
        }
        count++; //increment the count
    }
}

/** Send pattern to turn off some of segments in 7-segment display
 */
void sendPattern(unsigned char pattern, unsigned char pattern2){
    PORTC = pattern & 63; //send pattern1 to digit 1
    PORTB = pattern >> 5; //shift the pattern to the right 5 spaces so that RB0 can be used as interrupt
    
    PORTBbits.RB2 = 0; //turn on digit1
    PORTBbits.RB3 = 1;//turn off digit2
    
    __delay_ms(10); // Delay for 10 milliseconds
    
    PORTBbits.RB2 = 1; //turn off digit1
    
    PORTC = pattern2 & 63; //send pattern1 to digit 2
    PORTB = pattern2 >> 5; //must bitwise-OR with PORTB to retain LEDs that are on
    
    PORTBbits.RB2 = 1; //turn off digit1
    PORTBbits.RB3 = 0; //turn on digit2
    
    __delay_ms(10); // Delay for 10 milliseconds
    
    PORTBbits.RB3 = 1;//turn off digit2
}

/** Initializes appropriate ports, including TRIS registers and clearing Ports where necessary
 */
void initialize() {
    TRISC = 0; //set all of Port C to be outputs
    TRISBbits.TRISB0 = 1; //set the first three bits of Port B to be Outputs
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    
    PORTC = 0x00; //clear Ports B and C
    PORTB = 0x00;
    
    PORTBbits.RB2 = 1; //turn off digit1
    PORTBbits.RB3 = 1; //turn off digit2
    
    TRISD = 0; //set Port D to be output, so that they can be read by LCD
    
    TRISEbits.TRISE0 = 0; //set first two bits of Port E to be outputs
    TRISEbits.TRISE1 = 0;
    
    INTCONbits.GIE = 1; //enable interrupts globally
    
    INTCONbits.PEIE = 1; //enable peripheral interrupts
    
    INTCONbits.INT0E = 1; //enable INT0 interrupt
    
    INTCONbits.INT0F = 0; //clear flag for INT0
    
    INTCON2bits.INTEDG0 = 1; //set to rising edge trigger
    
    TRISAbits.RA1 = 0; //set picboard LED ports to be outputs so that LEDs can be controlled
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    
    LATAbits.LA1 = 0; //turn off the LEDs on the picboard
    LATAbits.LA2 = 0; 
    LATAbits.LA3 = 0; 
    
}
