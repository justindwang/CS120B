/*	Author: Justin Wang
 *	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1d7mYZwui1glSUeWnKv_j2V7ecn1QbEF5/view?usp=sharing
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

enum states { start, pause, s1, s2, s3, s4} state;
unsigned char tmpB = 0x00;

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

void tick() {
	switch(state) {
		case start:
    			state = s1;
    			break;
		case s1:
   			if (~PINA & 0x01) {
      				state = pause;} 
			else {
    				state = s2;}
    			break;
    		case s2:
    			if (~PINA & 0x01) {
      				state = pause;} 
			else {
    				state = s3;}
    			break;
    		case s3:
    			if (~PINA & 0x01) {
      				state = pause;}
			else {
   				state = s4;}
    			break;
		case s4:
    			if (~PINA & 0x01) {
      				state = pause;}
			else {
   				state = s1;}
    			break;
    		case pause:
    			if (~PINA & 0x01) {
    				state = s1;}
			else {
    				state = pause;}
    			break;
    		default:
    			state = start;
    			break;
  	}
  	switch(state) {
    		case start:
    			break;
    		case s1:
    			tmpB = 0x01;
    			break;
    		case s2:
    			tmpB = 0x02;
    			break;
    		case s3:
    			tmpB = 0x04;
    			break;
		case s4:
    			tmpB = 0x02;
    			break;
    		case pause:
    			tmpB = tmpB;
    			break;
    		default:
    			state = start;
    			break;
  	}
}

int main(void) {
	/* Insert DDR and PORT initializations */
    	DDRA = 0x00;
   	PORTA = 0xFF;
    	DDRB = 0xFF;
    	PORTB = 0x00;
    	TimerSet(300); 
    	TimerOn(); 
    	state = start; 
    	/* Insert your solution below */
    	while (1) {
      		tick();
      		while (!TimerFlag);
      		TimerFlag = 0;
      		PORTB = tmpB;
    	}
    	return 1;
}
