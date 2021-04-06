/*	Author: jwang592
 *  Partner(s) Name: Justin Wang
 *	Lab Section:
 *	Assignment: Lab 2  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
    	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s
	unsigned char cntavail = 0x00; // Temporary variable to hold the value of B
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	unsigned char tmpC = 0x00;


	while(1) {
		cntavail = 0;
		tmpA = PINA & 0x0F;
		tmpC = 0;


		if(!((tmpA & 0x01) && 0x01)) {
	
			cntavail+=1;
		}
		if(!((tmpA & 0x02) && 0x01)) {
			
                        cntavail+=1;
                }
		if(!((tmpA & 0x04) && 0x01)) {
			
                        cntavail+=1;
                }
		if(!((tmpA & 0x08) && 0x01)) {
			
                        cntavail+=1;
                }
		PORTC = cntavail;
		if(cntavail ==  0){
			PORTC = 0x80; // Set C7 to 1 if no spots were available
		}

	}
	return 0;
}
