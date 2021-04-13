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

enum states {START, CODE0, CODE1, CODE2, CODE3, SWITCH} state;
unsigned char pound = 0x00;
unsigned char X1 = 0x00;
unsigned char Y = 0x00;
unsigned char X2 = 0x00;
unsigned char tmpA7 = 0x00;
unsigned char tmpB = 0x00;

void tick(void){
	// array?
	pound = PINA & 0x04;
	X1 = PINA & 0x01;
	Y = PINA & 0x02;
	X2 = PINA & 0x01;

	tmpA7 = PINA & 0x80;

	switch(state){
		case START:
			state = CODE0;
			break;
		case CODE0:
			if(X1 || Y){
				state = CODE0;}
			if(tmpB && tmpA7){
				state = SWITCH;}
			else if(pound){
				state = CODE1;}
			else{
				state = CODE0;}
			break;
		case CODE1:
			if(Y || tmpA7){
				state = CODE0;}
			else if(!pound && X1){
				state = CODE2;}
			else if(pound && !X1){
				state = CODE1;}
			else if(!pound && !X1){
				state = CODE1;}
			else{
				state = CODE0;}
			break;
		case CODE2:
			if(pound || tmpA7){
				state = CODE0;}
			else if(!X1 && Y){
				state = CODE3;}
			else if(X1 && !Y){
				state = CODE2;}
			else if(!X1 && !Y){
				state = CODE2;}
			else{
				state = CODE0;}
			break;
		case CODE3:
			if(pound || tmpA7){
				state = CODE0;}
			else if(!Y && X2){
				state = SWITCH;}
			else if(Y && !X2){
				state = CODE3;}
			else if(!Y && !X2){
				state = CODE3;}
			else{
				state = CODE0;}
			break;
		case SWITCH:
			state = CODE0;
			break;
		default:
			break;
	}
	switch(state){
		case START:
			break;
		case CODE0:
			break;
		case CODE1:
			break;
		case CODE2:
			break;
		case CODE3:
			break;
		case SWITCH:
			if(tmpB){
				tmpB=0;}
			else{
				tmpB = 1;}
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
