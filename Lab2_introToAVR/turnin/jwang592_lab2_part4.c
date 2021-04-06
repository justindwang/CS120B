/*	Author: jwang592
 *  Partner(s) Name: Justin Wang
 *	Lab Section:
 *	Assignment: Lab 2  Exercise 4
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
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0x00; PORTC = 0xFF;
	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as outputs, initialize to 0s
	unsigned short totalWeight = 0x0000;
	unsigned char tmpA = 0x00;
	unsigned char tmpB = 0x00;
	unsigned char tmpC = 0x00;
	unsigned char tmpD = 0x00;


	while(1) {
		tmpA = PINA;
		tmpB = PINB;
		tmpC = PINC;
		tmpD = 0x00;

		totalWeight = tmpA + tmpB + tmpC;
		tmpD = totalWeight >> 2; // Approximation of totalWeight's values (totalWeight[9:4]). Shifting by 2 guarantees that there is no overflow if the maximum input is inputted
		tmpD = tmpD & 0xFC; // Set D0 and D1 to 0
		if(totalWeight > 0x008C) { // Check if totalWeight exceeds 180
			tmpD = tmpD | 0x01;
		}
		if(tmpA>tmpC){ // Check if difference between A and C is greater than 80
			if(tmpA-tmpC > 0x50){
				tmpD = tmpD | 0x02;
			}
		}
		else{
			if(tmpC-tmpA > 0x50){
				tmpD = tmpD | 0x02;
			}
		}
		PORTD = tmpD;
	}
	return 0;
}
