/*	Author: Justin Wang
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
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

enum states { start, s1, s2, s3 } state;
enum states2 { start2, on, off } state2;
enum states3 { start3, combine } state3;
unsigned char tmpB1 = 0x00;
unsigned char tmpB2 = 0x00;
unsigned long count1 = 300;
unsigned long count2 = 1000;

void tick() {
  	switch(state) {
    		case start:
    			state = s1;
    			break;
    		case s1:
    			state = s2;
    			break;
    		case s2:
    			state = s3;
    			break;
    		case s3:
    			state = s1;
    			break;
  	}

  	switch(state) {
    		case start:
			tmpB1 = 0x00;
    			break;
    		case s1:
    			tmpB1 = 0x01;
    			break;
    		case s2:
    			tmpB1 = 0x02;
    			break;
    		case s3:
    			tmpB1 = 0x04;
    			break;
  	}
}

void tick2() {
  	switch(state2) {
    		case start2:
    			state2 = off;
    			break;
    		case on:
  			state2 = off;
    			break;
    		case off:
    			state2 = on;
    			break;
  	}

  	switch(state2) {
    		case start2:
    			tmpB2 = 0x00;
    			break;
    		case on:
    			tmpB2 = 0x08;
    			break;
    		case off:
    			tmpB2 = 0x00;
    			break;
  	}
}

void tick3() {
  	switch(state3) {
    		case start3:
    			state3 = combine;
    			break;
    		case combine:
    			state3 = combine;
    			break;
  	}

  	switch(state3) {
    		case start3:
    			PORTB = 0x00;
    			break;
    		case combine:
    		PORTB = tmpB2 | tmpB1;
    		break;
  	}
}




int main(void) {
	/* Insert DDR and PORT initializations */
    	DDRB = 0xFF;
    	PORTB = 0x00;
    	TimerSet(100); //set timer here
    	TimerOn(); //turn on timer
    	state = start; 
    	state2 = start2;
    	state3 = start3;
    	/* Insert your solution below */
    	while (1) {
		if(count1 >= 300){
      			tick();
			count1 = 0;}
      		if(count2 >= 1000){
			tick2();
			count2 = 0;}
      		tick3();
      		while (!TimerFlag);
      		TimerFlag = 0;
		count1 += 100;
		count2 += 100;
    	}
    	return 1;
}
