/*	Author: jwang592
 *  	Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1i2HTIksvsz2kW3Pph2Mj2lhL-UsyHI-g/view?usp=sharing
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

enum states {START, INC, DEC, RESET, HOLDINC, HOLDDEC} state;
unsigned char count = 0x00;
unsigned char loop = 0x00;

#define tmpA0 (~PINA & 0x01)
#define tmpA1 (~PINA & 0x02)

void tick(void){
	switch(state){
		case START:
			if(tmpA0 && tmpA1){
				state = RESET;}
			else if(tmpA0){
				state = INC;}
			else if(tmpA1){
				state = DEC;}
			else{
				state = START;}
			break;
		case INC:
			if(tmpA0 && tmpA1){
				state = RESET;}
			else if(tmpA0){
				state = HOLDINC;}
			
			else{
				state = START;}
			break;
		case DEC:
			if(tmpA0 && tmpA1){
				state = RESET;}
			else if(tmpA1){
				state = HOLDDEC;}
			
			else{
				state = START;}
			break;
		case RESET:
			state = START;
			break;
		case HOLDINC:
			if(tmpA0 && tmpA1){
				state = RESET;}
			else if (tmpA0){
				state = HOLDINC;}
			else{
				state = START;}
			break;
		case HOLDDEC:
			if(tmpA0 && tmpA1){
				state = RESET;}
			else if (tmpA1){
				state = HOLDDEC;}
			else{
				state = START;}
			break;
		default:
			break;
	}
	switch(state){
		case START:
			loop = 0x00;
			break;
		case INC:
			if (count < 9){
				count = count + 1;}
			break;
		case DEC:
			if (count > 0){
				count = count - 1;}
			break;
		case RESET:
			count = 0x00;
			break;
		case HOLDINC:
			if(loop < 10){
				loop = loop + 1;}
			else{
				if (count < 9){
				count = count + 1;}
				loop = 0x00;}
			break;
		case HOLDDEC:
			if(loop < 10){
				loop = loop + 1;}
			else{
				if (count > 0){
				count = count - 1;}
				loop = 0x00;}
			break;
		default:
			break;
	}
}


int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; DDRB = 0xFF; PORTA = 0xFF; PORTB = 0x00;
	state = START;
	count = 7;
	TimerSet(100); 
	TimerOn(); 
	state = START; 
    	while (1) {
		tick();
  		while (!TimerFlag);
  		TimerFlag = 0;
        	PORTB = count;
    	}
    return 1;
}
