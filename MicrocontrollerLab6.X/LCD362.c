#include "LCD362.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 10000000
#endif

void sendNibble(unsigned char nibble) {
    // Set RD4-7 without affecting RD0-3
    LATD = (LATD & 0x0F) + (nibble & 0xF0);
    __delay_ms(1);
    // Note: data is latched on falling edge of pin E
    PIN_E = 0;
    __delay_us(10);
    PIN_E = 1;
    __delay_ms(1); // Enough time even for slowest command
}

/** Send a command byte to the LCD
 */
void sendCommand(unsigned char byte) {
    PIN_RS = 0;
    // Send 1st nibble
    sendNibble(byte & 0xF0);
    // Send 2nd nibble
    sendNibble(byte << 4);
}

/** Send a byte to display on the LCD
 */
void sendData(unsigned char byte) {
    PIN_RS = 1;
    // Send 1st nibble
    sendNibble(byte & 0xF0);
    // Send 2nd nibble
    sendNibble(byte << 4);
}

/** Clear the LCD screen
 */
void clearLCD() {
    sendCommand(0x01);
}

void initLCD() {
    PIN_RS = 0;
    PIN_E = 1;
    // Initialization
    __delay_ms(16); // must be more than 15 ms
    // Find what each command does
    sendCommand(0b00110011);
    sendCommand(0b00110010);
    sendCommand(0b00101000); 
    sendCommand(0b00000001); // clear display
    sendCommand(0b00001100); // turn off cursor, turns on display
    sendCommand(0b00000110); // increment cursor automatically
}

/** Write the string to the LCD
 */
void writeString(const unsigned char *s) {
    int i;
    for (i = 0; s[i] != '\0'; ++i) {
        writeChar(s[i]);
    }
}

/** Write the ASCII character to the LCD
 */
void writeChar(unsigned char c) {
    sendData(c);
}

/** Set the cursor position
 */
void setCursor(unsigned char c) {
    sendCommand(c);
}

/* row should be 1 or 2, column should be 1-8
 */
void setCursor2(unsigned char row, unsigned char column) {
    unsigned char temp, z, y;
    if (row == 1) {
        temp = 0x80 + column - 1;
        sendCommand(temp);
    } else if (row == 2) {
        temp = 0xC0 + column - 1;
        sendCommand(temp);
    }

}


/* Power function return the result of an exponential operation
 * It takes in two parameters
 *      A. The base which is the base of the exponent
 *      B. The power which is the power of the exponent
 * 1. It then checks the if the power if equal to zero
 *      A.If it is, then return 1
 *      B.Otherwise continue operation
 * 2. Store the base into a integer value result
 * 3. Then loop power-1 times and set result = result * _base
 * 4. Finally return result
 */
int pow(int _base, int power){
    if(power == 0){
        return 1;
    }
    int result = _base;
    for(int i = 0; i < power-1; i++){
        result = result * _base;
    }
    return result;
}

/** Write num as binary starting at cursor position 
 */
void writeAsBinary(int num, unsigned char cursor) {
    setCursor(cursor);
    unsigned char number;
    for(int i = 7; i >= 0; i--){
        number = (num & (unsigned char)pow(2, i)) >> i; // get value of bit
        writeChar(number + 0x30); // send character to current position of cursor
    }
}

/** Write decimal as hex in row number. This is a little more general
 *  than the function below, but both can be used to display a hex value. 
 */
void decimalToHex(long dec, int row) {
    int i, j = 0;
    // Initialize array with 0's.
    unsigned char hexNum[6] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
    long quotient, remainder;
    unsigned char cursor;

    quotient = dec;
    while (quotient != 0) {
        remainder = quotient % 16;
        if (remainder < 10) {
            hexNum[j++] = 0x30 + remainder;
        } else {
            hexNum[j++] = 0x37 + remainder;
        }
        quotient /= 16;
    }
    // Set cursor so there is enough room to accommodate the displayed value
    // in the "right" area of the display.
    if (j == 1) j = 2; // This is to show at least two hex digits as in 0x0A
    if (row == 1) {
        cursor = 0x80 + 8 - j - 2; // subtract 2 to accommodate "0x" 
    } else {
        cursor = 0xC0 + 8 - j - 2; // subtract 2 to accommodate "0x"
    }
    setCursor(cursor);
    // Write as 0xXXXX to indicate hex value
    writeChar(0x30); // Write a 0;
    writeChar('x'); // Write x character for Hex
    for (i = j - 1; i >= 0; i--) {
        writeChar(hexNum[i]);
    }
}

/** Write a two digit hex value at row number
 */
void writeHexToLCD(unsigned char hex, int row) {
    unsigned char temp_L;
    unsigned char temp_H;

    temp_H = hex & 0xF0;
    temp_H = temp_H >> 4;
    if (temp_H > 9) {
        temp_H += 0x37;
    } else {
        temp_H += 0x30;
    }

    temp_L = hex & 0x0F;
    if (temp_L > 9) {
        temp_L += 0x37;
    } else {
        temp_L += 0x30;
    }

    unsigned char cursor = row == 1 ? 0x85 : 0xC5;
    setCursor(cursor);
    writeChar(temp_H);
    writeChar(temp_L);
    writeChar('h');
}

/** Scroll text to the right
 */
void shiftLCDRight() {
    sendCommand(0x1C);
}

/** Scroll text to the left
 */
void shiftLCDLeft() {
    sendCommand(0x18);
}

/** Write an integer
 */
void writeInteger(int v) {
    unsigned char buf[8];
    sprintf(buf, "%i", v);
    writeString(buf);
}

// Write a float
 
void writeFloat(float f) {
    unsigned char buf11[8];
    sprintf(buf11, "%f", f);
    writeString(buf11);
}
