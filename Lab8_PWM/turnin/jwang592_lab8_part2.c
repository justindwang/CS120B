/*	Author: Justin Wang
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1m2g8YKzYYHOhy0t9qV3OXGcYcBXXauO7/view?usp=sharing
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
enum states { off, on, hold1, hold2 } state;
enum states2 { note, inc, dec, wait1, wait2 } state2;
unsigned char play;
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
			if(tmpA == 0x01){
				state = hold2;}
			else{
				state = on;}
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
	}

	switch(state) {
		case off:
			play = 0;
			break;
		case on:
			play = 1;
			break;
		case hold1:
			break;
		case hold2:
			break;
	}
}

void tick2() {
	switch(state2) {
		case note:
			switch(tmpA) {
				case 0x01:
					state2 = note;
					break;
				case 0x02:
					state2 = wait1;
					break;
				case 0x04:
					state2 = wait2;
					break;
				default:
					state2 = note;
					break;
			}
			break;
		case inc:
			state2 = note;
			break;
		case dec:
			state2 = note;
			break;
		case wait1:
			if (tmpA == 0x02) {
				state2 = wait1;}
			else {
				state2 = inc;}
			break;
		case wait2:
			if (tmpA == 0x04) {
				state2 = wait2;}
			else {
				state2 = dec;}
			break;
	}
	switch(state2) {
		case note:
			if(play == 1){
				set_PWM(array[count]);}
			else{
				set_PWM(0);}
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
	state2 = note;
	count = 0;
	while(1) {
		tick();
		tick2();
	}
}
