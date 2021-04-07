/*	Author: jwang592
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 3  Exercise 1
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
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	unsigned char tmpA, tmpB, count = 0x00;
	
	while (1) {
		tmpA = PINA & 0xFF;
		tmpB = PINB & 0xFF;
		count = 0x00;
		if((tmpA & 0x01) && 0x01){
			count+=1;}
		if((tmpA & 0x02) && 0x01){
                        count+=1;}
		if((tmpA & 0x04) && 0x01){
                        count+=1;}
		if((tmpA & 0x08) && 0x01){
                        count+=1;}
		if((tmpA & 0x10) && 0x01){
                        count+=1;}
		if((tmpA & 0x20) && 0x01){
                        count+=1;}
		if((tmpA & 0x40) && 0x01){
                        count+=1;}
		if((tmpA & 0x80) && 0x01){
                        count+=1;}
		if((tmpB & 0x01) && 0x01){
                        count+=1;}
		if((tmpB & 0x02) && 0x01){
                        count+=1;}
		if((tmpB & 0x04) && 0x01){
                        count+=1;}
		if((tmpB & 0x08) && 0x01){
                        count+=1;}
		if((tmpB & 0x10) && 0x01){
                        count+=1;}
		if((tmpB & 0x20) && 0x01){
                        count+=1;}
		if((tmpB & 0x40) && 0x01){
                        count+=1;}
		if((tmpB & 0x80) && 0x01){
                        count+=1;}
		PORTC = count;	
	}
	return 0;
}
