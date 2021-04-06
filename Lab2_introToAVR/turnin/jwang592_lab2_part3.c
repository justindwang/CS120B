/*	Author: jwang592
 *  Partner(s) Name: Justin Wang
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
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
			tmpC = tmpC | 0x01;
			cntavail+=1;
		}
		if(!((tmpA & 0x02) && 0x01)) {
			tmpC = tmpC | 0x02;
                        cntavail+=1;
                }
		if(!((tmpA & 0x04) && 0x01)) {
			tmpC = tmpC | 0x04;
                        cntavail+=1;
                }
		if(!((tmpA & 0x08) && 0x01)) {
			tmpC = tmpC | 0x08;
                        cntavail+=1;
                }
		if(cntavail ==  0){
			tmpC = 0x80;
		}	
		PORTC = tmpC;

	}
	return 0;
}
