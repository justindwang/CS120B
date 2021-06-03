/*	Author: Justin Wang
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 11
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
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1ms
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B 	= 0x0B;	// bit3 = 1: CTC mode (clear timer on compare)
					// bit2bit1bit0=011: prescaler /64
					// 00001011: 0x0B
					// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
					// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A 	= 125;	// Timer interrupt will be generated when TCNT1==OCR1A
					// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
					// So when TCNT1 register equals 125,
					// 1 ms has passed. Thus, we compare to 125.
					// AVR timer interrupt mask register

	TIMSK1 	= 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1 = 0;

	// TimerISR will be called every _avr_timer_cntcurr milliseconds
	_avr_timer_cntcurr = _avr_timer_M;

	//Enable global interrupts
	SREG |= 0x80;	// 0x80: 1000000
}

void TimerOff() {
	TCCR1B 	= 0x00; // bit3bit2bit1bit0=0000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect)
{
	// CPU automatically calls when TCNT0 == OCR0 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; 			// Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { 	// results in a more efficient compare
		TimerISR(); 				// Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

typedef struct task{
	// Tasks should have members that include: state, period,
	//a measurement of elapsed time, and a function pointer.
	signed 	 char state; 		//Task's current state
	unsigned long period; 		//Task period
	unsigned long elapsedTime; 	//Time elapsed since last task tick
	int (*TickFct)(int); 		//Task tick function
} task;


unsigned char rows[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char t_rows[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char block_r1[4] = {0x10, 0x10, 0x18, 0x08};
unsigned char block_r2[4] = {0x10, 0x18, 0x18, 0x10};

unsigned char cb = 0;
unsigned char cb_pos1 = 0x10;
unsigned char cb_pos2 = 0x10;
unsigned char cb_bot_pt = 1;

void transpose(){
    t_rows[0] = 0x00; t_rows[1] = 0x00; t_rows[2] = 0x00; t_rows[3] = 0x00; t_rows[4] = 0x00;
    t_rows[0] = ((rows[0]&0x01)<<7)|((rows[1]&0x01)<<6)|((rows[2]&0x01)<<5)|((rows[3]&0x01)<<4)|((rows[4]&0x01)<<3)|((rows[5]&0x01)<<2)|((rows[6]&0x01)<<1)|((rows[7]&0x01)<<0);
    t_rows[1] = ((rows[0]&0x02)<<6)|((rows[1]&0x02)<<5)|((rows[2]&0x02)<<4)|((rows[3]&0x02)<<3)|((rows[4]&0x02)<<2)|((rows[5]&0x02)<<1)|((rows[6]&0x02)<<0)|((rows[7]&0x02)>>1);
    t_rows[2] = ((rows[0]&0x04)<<5)|((rows[1]&0x04)<<4)|((rows[2]&0x04)<<3)|((rows[3]&0x04)<<2)|((rows[4]&0x04)<<1)|((rows[5]&0x04)<<0)|((rows[6]&0x04)>>1)|((rows[7]&0x04)>>2);
    t_rows[3] = ((rows[0]&0x08)<<4)|((rows[1]&0x08)<<3)|((rows[2]&0x08)<<2)|((rows[3]&0x08)<<1)|((rows[4]&0x08)<<0)|((rows[5]&0x08)>>1)|((rows[6]&0x08)>>2)|((rows[7]&0x08)>>3);
    t_rows[4] = ((rows[0]&0x10)<<3)|((rows[1]&0x10)<<2)|((rows[2]&0x10)<<1)|((rows[3]&0x10)<<0)|((rows[4]&0x10)>>1)|((rows[5]&0x10)>>2)|((rows[6]&0x10)>>3)|((rows[7]&0x10)>>4);
}

void set_rows(unsigned char* arr, unsigned char s0, unsigned char s1, unsigned char s2,unsigned char s3, unsigned char s4, unsigned char s5, unsigned char s6, unsigned char s7){
    arr[0] = s0; arr[1] = s1; arr[2] = s2; arr[3] = s3; arr[4] = s4; arr[5] = s5; arr[6] = s6; arr[7] = s7;
} 

void remove_trail(){
    unsigned char t = rows[cb_bot_pt - 1];
    if((cb_pos1 & 0x10)&(t & 0x10)){
        rows[cb_bot_pt - 1] = rows[cb_bot_pt - 1] & 0x0F;
    }
    if((cb_pos1 & 0x08)&(t & 0x08)){
        rows[cb_bot_pt - 1] = rows[cb_bot_pt - 1] & 0x17;
    }
    if((cb_pos1 & 0x04)&(t & 0x04)){
        rows[cb_bot_pt - 1] = rows[cb_bot_pt - 1] & 0x1B;
    }
    if((cb_pos1 & 0x02)&(t & 0x02)){
        rows[cb_bot_pt - 1] = rows[cb_bot_pt - 1] & 0x1D;
    }
    if((cb_pos1 & 0x01)&(t & 0x01)){
        rows[cb_bot_pt - 1] = rows[cb_bot_pt - 1] & 0x1E;
    }
    t = rows[cb_bot_pt];
    if((cb_pos2 & 0x10)&(t & 0x10)){
        rows[cb_bot_pt] = rows[cb_bot_pt] & 0x0F;
    }
    if((cb_pos2 & 0x08)&(t & 0x08)){
        rows[cb_bot_pt] = rows[cb_bot_pt] & 0x17;
    }
    if((cb_pos2 & 0x04)&(t & 0x04)){
        rows[cb_bot_pt] = rows[cb_bot_pt] & 0x1B;
    }
    if((cb_pos2 & 0x02)&(t & 0x02)){
        rows[cb_bot_pt] = rows[cb_bot_pt] & 0x1D;
    }
    if((cb_pos2 & 0x01)&(t & 0x01)){
        rows[cb_bot_pt] = rows[cb_bot_pt] & 0x1E;
    }
}

unsigned char bot_clear(){
    if(cb_bot_pt == 7){
        return 0x00;
    }
    unsigned char t = rows[cb_bot_pt + 1];
    unsigned char x = 0x00;
    x = ((cb_pos2 & 0x10) & (t & 0x10)) | ((cb_pos2 & 0x08) & (t & 0x08)) | ((cb_pos2 & 0x04) & (t & 0x04)) | ((cb_pos2 & 0x02) & (t & 0x02)) | ((cb_pos2 & 0x01) & (t & 0x01));
    if(x){
        return 0x00;
    }
    else{
        return 0x01;
    }
}

unsigned char top_clear(){
    if(rows[1] & 0x18){
        return 0x00;
    }
    return 0x01;
}

enum temp_states {grav, prep_next, reset};
int tick_grav(int state){
    switch(state){
        case grav:
            if(bot_clear()){
                state = grav;
            }
            else{
                state = prep_next;
            }
            break;
        case prep_next:
            break;
        case reset:
            state = reset;
        default:
            break;
    }
    switch(state){
        case grav:
            if(bot_clear()){
                remove_trail();
                rows[cb_bot_pt] = rows[cb_bot_pt] | cb_pos1;
                rows[cb_bot_pt + 1] = rows[cb_bot_pt + 1] | cb_pos2;
                cb_bot_pt++;
            }
            break;
        case prep_next:
            if(cb < 4){
                cb++;
            }
            else{
                cb = 0;
            }
            cb_pos1 = block_r1[cb];
            cb_pos2 = block_r2[cb];
            cb_bot_pt = 1;
            if(top_clear()){
                rows[0] = rows[0] | cb_pos1;
                rows[1] = rows[1] | cb_pos2;
                state = grav;
            }
            else{
                state = reset;
            } 
            break;
        case reset:
            rows[6] = 0x1F;
            rows[7] = 0x1F;
            break;
        default:
            break;
    }
    return state;
}

enum disp_states {s0, s1, s2, s3, s4};
int disp_tick(int state) {

	// Local Variables
	static unsigned char pattern = 0x80;	// LED pattern - 0: LED off; 1: LED on
	static unsigned char row = 0xFE;  	// Row(s) displaying pattern.
							// 0: display pattern on row
							// 1: do NOT display pattern on row
	// Transitions
	switch (state) {
		case s0:
            state = s1;
			break;
        case s1:
            state = s2;
            break;
        case s2:
            state = s3;
            break;
        case s3:
            state = s4;
            break;
        case s4:
            state = s0;
            break;
		default:
			state = s0;
			break;
	}
	// Actions
	switch (state) {
		case s0:
			transpose();
            pattern = t_rows[0];
            row = 0xFE;
			break;
        case s1:
            pattern = t_rows[1];
            row = 0xFD;
            break;
        case s2:
            pattern = t_rows[2];
            row = 0xFB;
            break;
        case s3:
            pattern = t_rows[3];
            row = 0xF7;
            break;
        case s4:
            pattern = t_rows[4];
            row = 0xEF;
            break;
		default:
			break;
	}
    PORTC = pattern;	// Pattern to display
	PORTD = row;		// Row(s) displaying pattern	
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;	
    /* Insert your solution below */
	task tasks[2];
	tasks[0].state = s0;
	tasks[0].period = 2;
	tasks[0].elapsedTime = 2;
	tasks[0].TickFct = &disp_tick;

    tasks[1].state = grav;
	tasks[1].period = 1000;
	tasks[1].elapsedTime = 1000;
	tasks[1].TickFct = &tick_grav;

	TimerSet(1);
	TimerOn();

	unsigned short i;
    while(1) {
	    for(i=0; i < 2; i++){
		    if(tasks[i].elapsedTime >= tasks[i].period){
			    tasks[i].state = tasks[i].TickFct(tasks[i].state);
			    tasks[i].elapsedTime = 0;
		    }
		    tasks[i].elapsedTime += 1;
	    }
	    while(!TimerFlag);
	    TimerFlag = 0;
    }
    return 1;
}
