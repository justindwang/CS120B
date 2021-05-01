/*	Author: Justin Wang
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 5  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *  
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 * 
 *  Demo Link: https://youtu.be/SXrHNIm5Bf8
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum states {start, s1, s2, s3, s4, s5, s6, s7, s8, wait1, wait2, wait3, wait4, wait5, wait6, wait7, wait8, wait9} state;
unsigned char tmpA = 0x00;

void tick(void){
	switch(state){
		case start:
			if(tmpA){
				PORTB = 0x01;
				state = wait1;}
			else{
				state = start;}
			break;
		case s1:
			if(tmpA){
				PORTB = 0x03;
				state = wait2;}
			else{
				state = s1;}
			break;
		case s2:
			if(tmpA){
				PORTB = 0x07;
				state = wait3;}
			else{
				state = s2;}
			break;
		case s3:
			if(tmpA){
				PORTB = 0x0E;
				state = wait4;}
			else{
				state = s3;}
			break;
		case s4:
			if(tmpA){
				PORTB = 0x1C;
				state = wait5;}
			else{
				state = s4;}
			break;
		case s5:
			if(tmpA){
				PORTB = 0x38;
				state = wait6;}
			else{
				state = s5;}
			break;
		case s6:
			if(tmpA){
				PORTB = 0x30;
				state = wait7;}
			else{
				state = s6;}
			break;
		case s7:
			if(tmpA){
				PORTB = 0x20;
				state = wait8;}
			else{
				state = s7;}
			break;
		case s8:
			if(tmpA){
				PORTB = 0x00;
				state = wait9;}
			else{
				state = s8;}
			break;
		case wait1:
			if(!tmpA){
				state = s1;}
			else{
				state = wait1;}
			break;
		case wait2:
			if(!tmpA){
				state = s2;}
			else{
				state = wait2;}
			break;
		case wait3:
			if(!tmpA){
				state = s3;}
			else{
				state = wait3;}
			break;
		case wait4:
			if(!tmpA){
				state = s4;}
			else{
				state = wait4;}
			break;
		case wait5:
			if(!tmpA){
				state = s5;}
			else{
				state = wait5;}
			break;
		case wait6:
			if(!tmpA){
				state = s6;}
			else{
				state = wait6;}
			break;
		case wait7:
			if(!tmpA){
				state = s7;}
			else{
				state = wait7;}
			break;
		case wait8:
			if(!tmpA){
				state = s8;}
			else{
				state = wait8;}
			break;
		case wait9:
			if(!tmpA){
				state = start;}
			else{
				state = wait9;}
			break;
		default:
			break;
	}
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	PORTB = 0x00;
	state = start;
	while(1){
		tmpA = ~PINA & 0x01;
		tick();
	}
	return 0;
}
