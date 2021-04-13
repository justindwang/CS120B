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

enum states {START, INC, DEC, RESET, WAIT_CHANGE} state;
unsigned char count = 0x00;
unsigned char last_state = 0x00; // 0 for reset, 1 for inc, 2 for dec

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
			state = WAIT_CHANGE;
			break;
		case DEC:
			state = WAIT_CHANGE;
			break;
		case RESET:
			state = WAIT_CHANGE;
			break;
		case WAIT_CHANGE:
			if(last_state == 0){
				if(PINA == 1 || PINA == 2){
					state = START;
				}
			}
			else if(last_state == 1){
				if(PINA == 0 || PINA == 2){
					state = START;
				}
			}
			else if(last_state == 2){
				if(PINA == 0 || PINA == 1){
					state = START;
				}
			}
			else{
				state = WAIT_CHANGE;
			}
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
			last_state = 1;
			break;
		case DEC:
			if (count > 0){
				count = count - 1;}
			last_state = 2;
			break;
		case RESET:
			count = 0x00;
			last_state = 0;
			break;
		case WAIT_CHANGE:
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
