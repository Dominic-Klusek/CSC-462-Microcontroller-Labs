/*
 * File:   Part2.c
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
int currentTMP = 0;
unsigned char thresholdTMP = 0xFF;

void __interrupt() intr(){
    if(PIR1bits.ADIF == 1){
        if(currentTMP >= thresholdTMP){
            LATAbits.LA1 = 1; //turn off the LEDs on the picboard
            LATAbits.LA2 = 1; 
            LATAbits.LA3 = 1; 
            setCursor(0xC0); //set cursor
            writeString("PANIC!"); //write Placeholder text
            __delay_ms(1000);
            LATAbits.LA1 = 0; //turn off the LEDs on the picboard
            LATAbits.LA2 = 0; 
            LATAbits.LA3 = 0; 
            clearLCD();
        }
        PIR1bits.ADIF = 0;
    }
}

void sendPattern(unsigned char pattern, unsigned char pattern2);
void initialize();
void changeforinterrupt();

void main(void) {
    initialize(); //initialize the microcontroller
    __delay_ms(10);
    initLCD(); //initialize the LCD
    
    clearLCD(); //clear the LCD of previous message
    unsigned char pressed;
    setCursor(0x80); //set cursor
    writeString("TMP: "); //write Placeholder text
        
    for(;;){
        pressed = getKeyPad(); //get key position from keypad
            
        pressed = getMappedKey(pressed); //get mapping of key
            
        //if the value of pressed is a number, store value into number1
        if(pressed >= 0 && pressed < 10){
            if(thresholdTMP != 0xFF){
                thresholdTMP = (thresholdTMP * 10) + pressed;
            } else{
                thresholdTMP = pressed;
            }
            //change displayed number on LCD
            setCursor(0x80);
            unsigned char buf[8];
            sprintf(buf, "TMP: %i", thresholdTMP);
            writeString(buf);
        } else if(pressed == 'E' && thresholdTMP != 0xFF){
            break;
        }
        //check if ENTER(D) was pressed
    }
    
    changeforinterrupt();
    
    for (;;) {
        ADCON0bits.GO_DONE = 1;
        __delay_ms(20);
        currentTMP = (int)(ADRESL/2.04);
        setCursor(0x80); //write RoomTEMP on first line
        writeString("RoomTEMP");
        setCursor(0xC0); //write RoomTEMP on first line
        LATBbits.LATB0 = (currentTMP >= thresholdTMP);
        writeInteger(currentTMP);
        writeChar(0xdf);
        writeChar('F');
        __delay_ms(1000);
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

void changeforinterrupt(){
    TRISA = 0xFF;
    TRISE = 0;
    TRISD = 0; //set Port D to be output, so that they can be read by LCD
    ADCON0 = 0x41; //set ADCON0 to be set for temperature converter
    ADCON1bits.ADFM = 1; // configure A/D converter
    ADCON1bits.ADCS2 = 1;
    ADCON1bits.PCFG0 = 0;
    ADCON1bits.PCFG1 = 0;
    ADCON1bits.PCFG2 = 0;
    ADCON1bits.PCFG3 = 0;
    INTCONbits.GIE = 1; //enable interrupts globally
    INTCONbits.PEIE = 1; //enable peripheral interrupts
    INTCON2bits.INTEDG0 = 1; //set to rising edge trigger
    
    IPR1bits.ADIP = 1; //enable A/D converter interrupt
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    
    TRISAbits.RA1 = 0; //set picboard LED ports to be outputs so that LEDs can be controlled
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    
    LATAbits.LA1 = 0; //turn off the LEDs on the picboard
    LATAbits.LA2 = 0; 
    LATAbits.LA3 = 0; 
}
