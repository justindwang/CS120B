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

enum states {START, INC, DEC, RESET} state;
unsigned char count = 0x00;

void tick(void){
	switch(state){
		case START:
			if(PINA == 3){
				state = RESET;}
			else if(PINA == 1){
				state = INC;}
			else if(PINA == 2){
				state = DEC;}
			else{
				state = START;}
			break;
		case INC:
			state = START;
			break;
		case DEC:
			state = START;
			break;
		case RESET:
			state = START;
			break;
		default:
			break;
	}
	switch(state){
		case START:
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
		default:
			break;
	}
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	state = START;
	count = 0x07;
	while(1){
		tick();
		PORTC = count;
	}
	return 0;
}
