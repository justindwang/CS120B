/*	Author: jwang592
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum states {START, LOCKED, WAIT_Y, UNLOCKED} state;
unsigned char X = 0x00;
unsigned char Y = 0x00;
unsigned char pound = 0x00;
unsigned char tmpA7 = 0x00;
unsigned char tmpB = 0x00;

void tick(void){
	X = PINA & 0x01;
	Y = PINA & 0x02;
	pound = PINA & 0x04;
	tmpA7 = PINA & 0x80;

	switch(state){
		case START:
			state = LOCKED;
			break;
		case LOCKED:
			if(X || Y || tmpA7){
				state = LOCKED;}
			else if(pound){
				state = WAIT_Y;}
			else{
				state = LOCKED;}
			break;
		case WAIT_Y:
			if(X || tmpA7){
				state = LOCKED;}
			else if(pound && Y){
				state = LOCKED;}
			else if(!pound && Y){
				state = UNLOCKED;}
			else if(pound && !Y){
				state = WAIT_Y;}
			else if(!pound && !Y){
				state = WAIT_Y;}
			else{
				state = LOCKED;}
			break;
		case UNLOCKED:
			if(tmpA7){
				state = LOCKED;}
			else{
				state = UNLOCKED;}
			break;
		default:
			break;
	}
	switch(state){
		case START:
			break;
		case LOCKED:
			tmpB = 0;
			break;
		case WAIT_Y:
			break;
		case UNLOCKED:
			tmpB = 1;
			break;
		default:
			break;
	}
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	state = START;
	while(1){
		tick();
		PORTB = tmpB;
	}
	return 0;
}
