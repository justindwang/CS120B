/*	Author: Justin Wang
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 5  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
        DDRA = 0x00; PORTA = 0xFF;
        DDRC = 0xFF; PORTC = 0x00;
        unsigned char tmpA, tmpC = 0x00;

        while (1) {
                tmpA = ~PINA & 0x0F;
		tmpC = 0x40;
                if(tmpA > 0){
			tmpC = tmpC | 0x20;}
		if(tmpA > 2){
			tmpC = tmpC | 0x10;}
		if(tmpA > 4){
			tmpC = tmpC | 0x08;}
		if(tmpA > 6){
			tmpC = tmpC | 0x04;}
		if(tmpA > 9){
			tmpC = tmpC | 0x02;}
		if(tmpA > 12){
			tmpC = tmpC | 0x01;}
		if( tmpA > 5){
			tmpC = tmpC & 0xBF;}
		PORTC = tmpC;
        }
	return 0;
}
