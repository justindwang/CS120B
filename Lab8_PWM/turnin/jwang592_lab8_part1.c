/*	Author: Justin Wang
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
 	static double current_frequency;
  	if (frequency != current_frequency) {
   		if (!frequency) { TCCR3B &= 0x08; }
    		else { TCCR3B |= 0x03; }
    		if (frequency < 0.954) { OCR3A = 0xFFFF; }
    		else if (frequency > 31250) { OCR3A = 0x0000; }
    		else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }
    		TCNT3 = 0;
    		current_frequency = frequency;
  	}
}

void PWM_on() {
  	TCCR3A = (1 << COM3A0);
  	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
  	set_PWM(0);
}

void PWM_off() {
  	TCCR3A = 0x00;
  	TCCR3B = 0x00;
}

#define tmpA (~PINA & 0x07)
enum states { c, d, e, wait } state;

void tick() {
	switch(state) {
		case wait:
			switch(tmpA) {
				case 0x01:
					state = c;
					break;
				case 0x02:
					state = d;
					break;
				case 0x04:
					state = e;
					break;
				default:
					state = wait;
					break;
			}
    			break;
		case c:
			if (tmpA == 0x01) {
				state = c;}
			else {
				state = wait;}
			break;
		case d:
			if (tmpA == 0x02) {
				state = d;}
			else {
				state = wait;}
			break;
		case e:
			if (tmpA == 0x04) {
				state = e;}
			else {
				state = wait;}
			break;
	}
	switch(state) {
		case wait:
			set_PWM(0);
			break;
		case c:
			set_PWM(261.63);
			break;
		case d:
			set_PWM(293.66);
			break;
		case e:
			set_PWM(329.63);
			break;
	}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	PWM_on();
	state = wait;
	while(1) {
		tick();
	}
  	return 1;
}
