/*	Author: Justin Wang
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #2
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
double array[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
enum states { off, on, hold1, hold2, note, inc, dec, wait1, wait2 } state;
unsigned char count;

void tick() {
	switch(state) {
		case off:
			if (tmpA == 0x01) {
				state = hold1;}
			else {
				state = off;}
			break;
		case on:
			state = note;
			break;
		case hold1:
			if (tmpA == 0x01) {
				state = hold1;}
			else {
				state = on;}
			break;
		case hold2:
			if (tmpA == 0x01) {
				state = hold2;}
			else {
				state = off;}
			break;
		case note:
			switch(tmpA) {
				case 0x01:
					state = hold2;
					break;
				case 0x02:
					state = wait1;
					break;
				case 0x04:
					state = wait2;
					break;
				default:
					state = note;
					break;
			}
			break;
		case inc:
			state = note;
			break;
		case dec:
			state = note;
			break;
		case wait1:
			if (tmpA == 0x02) {
				state = wait1;}
			else {
				state = inc;}
			break;
		case wait2:
			if (tmpA == 0x04) {
				state = wait2;}
			else {
				state = dec;}
			break;
	}

	switch(state) {
		case off:
			PWM_off();
			break;
		case on:
			break;
		case hold1:
			PWM_on();
			count = 0;
			break;
		case hold2:
			break;
		case note:
			set_PWM(array[count]);
			break;
		case inc:
			if (count < 7) { 
				count++;}
			break;
		case dec:
			if (count > 0) {
				count--;}
			break;
		case wait1:
			break;
		case wait2:
			break;
	}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	PWM_on();
	state = off;
	while(1) {
		tick();
	}
}
