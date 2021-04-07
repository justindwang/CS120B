/*	Author: jwang592
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 3  Exercise 5
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
	DDRB = 0xFE; PORTB = 0x00;
 	DDRD = 0x00; PORTD = 0xFF;
	unsigned char tmpB = 0x00;
	unsigned short tmpD, totalWeight = 0x00;
	
	while (1) {
		tmpB = PINB & 0x01;
		tmpD = PIND;
		tmpD = tmpD << 1;
		totalWeight = tmpD + tmpB;
		if(totalWeight > 69){
			PORTB = 0x02;
		}
		else if(totalWeight > 5){
			PORTB = 0x04;
		}
		else{
			PORTB = 0x00;
		}
	}
	return 0;
}
