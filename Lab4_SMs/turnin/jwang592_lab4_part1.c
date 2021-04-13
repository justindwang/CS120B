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

int main(void){
	enum states {START, WAIT, SWITCH} state;
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	state = START;
	PORTB = 0x01;
	while(1){
		switch(state){
			case START:
				if((PINA & 0x01) && 0x01){
					PORTB = 0x02;
					state = WAIT;}
				else{
					state = START;}
				break;
			case WAIT:
				if(!PINA){
					state = SWITCH;}
				else{
					state = WAIT;}
				break;
			case SWITCH:
				if((PINA & 0x01) && 0x01){
					PORTB = 0x01;
					state = WAIT;}
				else{
					state = SWITCH;}
				break;
		}
		// switch(state){
		// 	case START:
		// 		PORTB = 0x01;
		// 		break;
		// 	case WAIT0:
		// 		break;
		// 	case SWITCH:
		// 		PORTB = 0x02;
		// 		break;
		// 	case WAIT1:
		// 		break;
		// }
	}
	return 0;
}
