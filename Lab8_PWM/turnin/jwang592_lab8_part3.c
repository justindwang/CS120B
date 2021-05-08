/*	Author: Justin Wang
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/10d5YpIdFVUS-IsxnYv7W2fjSD5psNlk0/view?usp=sharing
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
  	TCCR1B = 0x0B;
  	OCR1A = 125;
  	TIMSK1 = 0x02;
  	TCNT1 = 0;
  	_avr_timer_cntcurr = _avr_timer_M;
  	SREG |= 0x80;
}

void TimerOff() {
  	TCCR1B = 0x00;
}

void TimerISR() {
  	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

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

#define tmpA (~PINA & 0x01)
double array[14] = {261.63, 349.23, 392.00, 415.30, 392.00, 349.23, 261.63, 349.23, 392.00, 415.30, 466.16, 415.30, 466.16, 523.25};
enum states { off, on, hold } state;
unsigned char count;
unsigned char note;

void tick() {
	switch(state) {
		case off:
			if (tmpA == 0x01)
			{
				count = 0; 
				note = 0;
				state = on;
			}
			break;
		case on:
			if (count == 26 && tmpA == 0x01) // hold button when end
			{
				state = hold; 
			}
			else if (count == 26)
			{
				state = off;
			}
			else
			{
				state = on;
			}
			break;
		case hold:
			if (tmpA == 0x00) //if release button
			{
				state = off;
			}
			else{
				state = hold;
			}
			break;
		default:
			break;
	}

	switch(state) {
		case off:
			set_PWM(0.00); 
			break;
		case on:
			if (count == 0)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 1)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 2)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 3)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 6)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 9)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 12)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 13) 
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 14)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 15)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 18)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 20)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 21)
			{
				set_PWM(array[note]);
				note++;
			}
			else if (count == 23)
			{
				set_PWM(array[note]);
				note++;
			}
			count++;
			break;
		case hold:
			set_PWM(0.00);
			break;
		default:
			break;
	}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	PWM_on();
	TimerSet(200);
	TimerOn();
	state = off;
	while(1) {
		tick();
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
