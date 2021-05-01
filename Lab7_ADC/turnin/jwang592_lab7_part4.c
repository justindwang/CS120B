/*	Author: Justin Wang
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 7  Exercise 4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/10oq8Z6Jtu57kxKf2R_0f2uWY2owlJuE5/view?usp=sharing
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
	
	//unsigned short MAX = 480;
	unsigned short inc = 60;
	unsigned char LED = 0x00;

	ADC_init();

	while(1){
		LED = 0x00;
		unsigned short x = ADC;
		if(x >= inc){
			LED = LED | 0x01;}
		if(x >= inc*2){
			LED = LED | 0x02;}
		if(x >= inc*3){
			LED = LED | 0x04;}
		if(x >= inc*4){
			LED = LED | 0x08;}
		if(x >= inc*5){
			LED = LED | 0x10;}
		if(x >= inc*6){
			LED = LED | 0x20;}
		if(x >= inc*7){
			LED = LED | 0x40;}
		if(x >= inc*8){
			LED = LED | 0x80;}
		PORTB = LED;
	}

    return 1;
}
