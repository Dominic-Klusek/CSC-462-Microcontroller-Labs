
#include "keypadHelper.h"
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 10000000
#endif

unsigned char getKeyPad() {
    unsigned char i, val, x, key = 0x00;

    PORTC = 0x01; // Start with Column 1
    while ((PORTB & 0x0F) == 0) {  // While no key is pressed

        PORTC = PORTC << 1; // Go to the next column
        key++;              // Column number
        if (key == 4) {
            PORTC = 0x01;   // Back to column 1
            key = 0;
        }
    }                       // key debounce
    __delay_ms(100);
    for (i = 0x01; i < 0x10; i = i << 1) { // Find the key pressed
        if ((PORTB & i) != 0) break;
        key = key + 4;
    }

    PORTC = 0x0F;
    while ((PORTC & MASK) != 0); // Wait until key is released
    __delay_ms(100);              // Key debounce
    return key;
}

unsigned char getMappedKey(unsigned char key){
    switch(key){
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            return 3;
        case 3:
            return '+';
        case 4:
            return 4;
        case 5:
            return 5;
        case 6:
            return 6;
        case 7:
            return '-';
        case 8:
            return 7;
        case 9:
            return 8;
        case 10:
            return 9;
        case 11:
            return '/';
        case 12:
            return '*';
        case 13:
            return 0;
        case 14:
            return 'C';
        case 15:
            return 'E';
        default:
            return -1;  
    }
}
