/*	Author: Justin Wang
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 7  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1T8bNILVJwXyzUfossVPAd91j-sG2_7hp/view?usp=sharing
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	ADC_init();

	while(1){
		unsigned short x = ADC;
		unsigned char tmpB = (char)x;
		unsigned char tmpD = (char)(x >> 8);
		PORTB = tmpB;
		PORTD = tmpD;
	}

    return 1;
}
