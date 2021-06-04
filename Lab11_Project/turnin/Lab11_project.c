/*	Author: Justin Wang
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 11
 *	Exercise Description: Tetris game adapted to work on 5x8 LED Matrix
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/10KGatgsSAAsfx5hCF0p_rSKYWJHirM27/view?usp=sharing
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

// Board representation
unsigned char rows[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char t_rows[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

// Block Sequence
unsigned char block_r1[30] = {0x10, 0x10, 0x18, 0x08, 0x18, 0x18, 0x10, 0x18, 0x08, 0x00, 0x08, 0x18, 0x10, 0x18, 0x18, 0x08, 0x10, 0x10, 0x18, 0x10, 0x18, 0x08, 0x10, 0x18, 0x18, 0x10, 0x10, 0x18, 0x10, 0x18};
unsigned char block_r2[30] = {0x10, 0x18, 0x18, 0x10, 0x18, 0x18, 0x08, 0x10, 0x18, 0x18, 0x08, 0x18, 0x08, 0x10, 0x00, 0x10, 0x10, 0x10, 0x18, 0x18, 0x18, 0x08, 0x08, 0x18, 0x10, 0x18, 0x10, 0x18, 0x08, 0x18};

// Tracking Values for current block
unsigned char cb = 0;
unsigned char cb_pos1 = 0x10;
unsigned char cb_pos2 = 0x10;
unsigned char cb_bot_pt = 1;
unsigned char pt_val = 0;

// Helper function for transposing 8x5 array to 5x8 array
void transpose(){
    t_rows[0] = 0x00; t_rows[1] = 0x00; t_rows[2] = 0x00; t_rows[3] = 0x00; t_rows[4] = 0x00;
    t_rows[0] = ((rows[0]&0x01)<<7)|((rows[1]&0x01)<<6)|((rows[2]&0x01)<<5)|((rows[3]&0x01)<<4)|((rows[4]&0x01)<<3)|((rows[5]&0x01)<<2)|((rows[6]&0x01)<<1)|((rows[7]&0x01)<<0);
    t_rows[1] = ((rows[0]&0x02)<<6)|((rows[1]&0x02)<<5)|((rows[2]&0x02)<<4)|((rows[3]&0x02)<<3)|((rows[4]&0x02)<<2)|((rows[5]&0x02)<<1)|((rows[6]&0x02)<<0)|((rows[7]&0x02)>>1);
    t_rows[2] = ((rows[0]&0x04)<<5)|((rows[1]&0x04)<<4)|((rows[2]&0x04)<<3)|((rows[3]&0x04)<<2)|((rows[4]&0x04)<<1)|((rows[5]&0x04)<<0)|((rows[6]&0x04)>>1)|((rows[7]&0x04)>>2);
    t_rows[3] = ((rows[0]&0x08)<<4)|((rows[1]&0x08)<<3)|((rows[2]&0x08)<<2)|((rows[3]&0x08)<<1)|((rows[4]&0x08)<<0)|((rows[5]&0x08)>>1)|((rows[6]&0x08)>>2)|((rows[7]&0x08)>>3);
    t_rows[4] = ((rows[0]&0x10)<<3)|((rows[1]&0x10)<<2)|((rows[2]&0x10)<<1)|((rows[3]&0x10)<<0)|((rows[4]&0x10)>>1)|((rows[5]&0x10)>>2)|((rows[6]&0x10)>>3)|((rows[7]&0x10)>>4);
}

// Assignment function for setting board values
void set_rows(unsigned char* arr, unsigned char s0, unsigned char s1, unsigned char s2,unsigned char s3, unsigned char s4, unsigned char s5, unsigned char s6, unsigned char s7){
    arr[0] = s0; arr[1] = s1; arr[2] = s2; arr[3] = s3; arr[4] = s4; arr[5] = s5; arr[6] = s6; arr[7] = s7;
} 

// Helper function for erasing the image of a block in rows[]
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

// Helper function that checks if the next iteration of gravity is cleared
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

// Helper function that checks if the game can continune
unsigned char top_clear(){
    if(rows[1] & 0x18){
        return 0x00;
    }
    return 0x01;
}

// Helper function that moves the current block to the left if allowed
void shift_left(){
    if(cb_pos1 & 0x10 || cb_pos2 & 0x10){
        return;
    }
    remove_trail();
    unsigned char t_1 = cb_pos1 << 1;
    unsigned char t_2 = cb_pos2 << 1;
    unsigned char t_3 = rows[cb_bot_pt - 1];
    unsigned char t_4 = rows[cb_bot_pt];
    unsigned char x = ((t_1 & 0x10) & (t_3 & 0x10)) | ((t_1 & 0x08) & (t_3 & 0x08)) | ((t_1 & 0x04) & (t_3 & 0x04)) | ((t_1 & 0x02) & (t_3 & 0x02)) | ((t_1 & 0x01) & (t_3 & 0x01));
    unsigned char y = ((t_2 & 0x10) & (t_4 & 0x10)) | ((t_2 & 0x08) & (t_4 & 0x08)) | ((t_2 & 0x04) & (t_4 & 0x04)) | ((t_2 & 0x02) & (t_4 & 0x02)) | ((t_2 & 0x01) & (t_4 & 0x01));
    if(x || y){
        rows[cb_bot_pt - 1] = rows[cb_bot_pt - 1] | cb_pos1;
        rows[cb_bot_pt] = rows[cb_bot_pt] | cb_pos2;
        return;
    }
    else{
        remove_trail();
        cb_pos1 = cb_pos1 << 1;
        cb_pos2 = cb_pos2 << 1;
        rows[cb_bot_pt - 1] = rows[cb_bot_pt - 1] | cb_pos1;
        rows[cb_bot_pt] = rows[cb_bot_pt] | cb_pos2;
    }
}

// Helper function that moves the current block to the right if allowed
void shift_right(){
    if(cb_pos1 & 0x01 || cb_pos2 & 0x01){
        return;
    }
    remove_trail();
    unsigned char t_1 = cb_pos1 >> 1;
    unsigned char t_2 = cb_pos2 >> 1;
    unsigned char t_3 = rows[cb_bot_pt - 1];
    unsigned char t_4 = rows[cb_bot_pt];
    unsigned char x = ((t_1 & 0x10) & (t_3 & 0x10)) | ((t_1 & 0x08) & (t_3 & 0x08)) | ((t_1 & 0x04) & (t_3 & 0x04)) | ((t_1 & 0x02) & (t_3 & 0x02)) | ((t_1 & 0x01) & (t_3 & 0x01));
    unsigned char y = ((t_2 & 0x10) & (t_4 & 0x10)) | ((t_2 & 0x08) & (t_4 & 0x08)) | ((t_2 & 0x04) & (t_4 & 0x04)) | ((t_2 & 0x02) & (t_4 & 0x02)) | ((t_2 & 0x01) & (t_4 & 0x01));
    if(x || y){
        rows[cb_bot_pt - 1] = rows[cb_bot_pt - 1] | cb_pos1;
        rows[cb_bot_pt] = rows[cb_bot_pt] | cb_pos2;
        return;
    }
    else{
        cb_pos1 = cb_pos1 >> 1;
        cb_pos2 = cb_pos2 >> 1;
        rows[cb_bot_pt - 1] = rows[cb_bot_pt - 1] | cb_pos1;
        rows[cb_bot_pt] = rows[cb_bot_pt] | cb_pos2;
    }
}

// Helper function that rotates the current block counterclockwise if allowed
void flip_block(){
    remove_trail();
    unsigned char t_1 = 0x00;
    unsigned char t_2 = 0x00;
    unsigned char t_3 = rows[cb_bot_pt - 1];
    unsigned char t_4 = rows[cb_bot_pt];
    unsigned char top = 0;
    unsigned char bot = 0;
    unsigned char first_visited = 0;

    if(cb_pos1 & 0x10){ t_2 = t_2 | 0x10; top = 1; }
    if(cb_pos2 & 0x10){ t_2 = t_2 | 0x08; bot = 1; }
    if(top || bot){ first_visited = 1; }
    top = 0; bot = 0;

    if((cb_pos1 & 0x08) && first_visited == 1){ t_1 = t_1 | 0x10; }
    if((cb_pos2 & 0x08) && first_visited == 1){ t_1 = t_1 | 0x08; }
    if((cb_pos1 & 0x08) && first_visited == 0){ t_2 = t_2 | 0x08; top = 1; }
    if((cb_pos2 & 0x08) && first_visited == 0){ t_2 = t_2 | 0x04; bot = 1; }
    if(top || bot){ first_visited = 1; }
    top = 0; bot = 0;

    if((cb_pos1 & 0x04) && first_visited == 1){ t_1 = t_1 | 0x08; }
    if((cb_pos2 & 0x04) && first_visited == 1){ t_1 = t_1 | 0x04; }
    if((cb_pos1 & 0x04) && first_visited == 0){ t_2 = t_2 | 0x04; top = 1; }
    if((cb_pos2 & 0x04) && first_visited == 0){ t_2 = t_2 | 0x02; bot = 1; }
    if(top || bot){ first_visited = 1; }
    top = 0; bot = 0;

    if((cb_pos1 & 0x02) && first_visited == 1){ t_1 = t_1 | 0x04; }
    if((cb_pos2 & 0x02) && first_visited == 1){ t_1 = t_1 | 0x02; }
    if((cb_pos1 & 0x02) && first_visited == 0){ t_2 = t_2 | 0x02; }
    if((cb_pos2 & 0x02) && first_visited == 0){ t_2 = t_2 | 0x01; }

    if(cb_pos1 & 0x01){ t_1 = t_1 | 0x02; }
    if(cb_pos2 & 0x01){ t_1 = t_1 | 0x01; }
    
    unsigned char x = ((t_1 & 0x10) & (t_3 & 0x10)) | ((t_1 & 0x08) & (t_3 & 0x08)) | ((t_1 & 0x04) & (t_3 & 0x04)) | ((t_1 & 0x02) & (t_3 & 0x02)) | ((t_1 & 0x01) & (t_3 & 0x01));
    unsigned char y = ((t_2 & 0x10) & (t_4 & 0x10)) | ((t_2 & 0x08) & (t_4 & 0x08)) | ((t_2 & 0x04) & (t_4 & 0x04)) | ((t_2 & 0x02) & (t_4 & 0x02)) | ((t_2 & 0x01) & (t_4 & 0x01));
    if(x || y){
        rows[cb_bot_pt - 1] = rows[cb_bot_pt - 1] | cb_pos1;
        rows[cb_bot_pt] = rows[cb_bot_pt] | cb_pos2;
        return;
    }
    else{
        cb_pos1 = t_1;
        cb_pos2 = t_2;
        rows[cb_bot_pt - 1] = rows[cb_bot_pt - 1] | cb_pos1;
        rows[cb_bot_pt] = rows[cb_bot_pt] | cb_pos2;
    }
}

// Helper function that clears fully filled rows and performs gravity on remaining rows
void clear_matched_rows(){
    if(rows[1] == 0x1F){
        set_rows(rows, 0x00, rows[0], rows[2], rows[3], rows[4], rows[5], rows[6], rows[7]);
        pt_val++;
    }
    if(rows[2] == 0x1F){
        set_rows(rows, 0x00, rows[0], rows[1], rows[3], rows[4], rows[5], rows[6], rows[7]);
        pt_val++;
    }
    if(rows[3] == 0x1F){
        set_rows(rows, 0x00, rows[0], rows[1], rows[2], rows[4], rows[5], rows[6], rows[7]);
        pt_val++;
    }
    if(rows[4] == 0x1F){
        set_rows(rows, 0x00, rows[0], rows[1], rows[2], rows[3], rows[5], rows[6], rows[7]);
        pt_val++;
    }
    if(rows[5] == 0x1F){
        set_rows(rows, 0x00, rows[0], rows[1], rows[2], rows[3], rows[4], rows[6], rows[7]);
        pt_val++;
    }
    if(rows[6] == 0x1F){
        set_rows(rows, 0x00, rows[0], rows[1], rows[2], rows[3], rows[4], rows[5], rows[7]);
        pt_val++;
    }
    if(rows[7] == 0x1F){
        set_rows(rows, 0x00, rows[0], rows[1], rows[2], rows[3], rows[4], rows[5], rows[6]);
        pt_val++;
    }
}

// State machine that performs one tick of gravity 
enum temp_states {grav, prep_next, reset, wait_new_game};
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
            break;
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
            if(cb < 30){
                cb++;
            }
            else{
                cb = 0;
            }
            cb_pos1 = block_r1[cb];
            cb_pos2 = block_r2[cb];
            cb_bot_pt = 1;
            if(top_clear()){
                clear_matched_rows();
                clear_matched_rows();
                rows[0] = rows[0] | cb_pos1;
                rows[1] = rows[1] | cb_pos2;
                state = grav;
            }
            else{
                state = reset;
            } 
            break;
        case reset:
            break;
        default:
            break;
    }
    return state;
}

#define LEFT (~PINA & 0x04)
#define RIGHT (~PINA & 0x02)
#define FLIP (~PINA & 0x01)


// State machine that handles inputs from player
enum input_states {wait, l_press, l_release, r_press, r_release, flip_press, flip_release};
int input_tick(int state){
    switch(state){
        case wait:
            if(LEFT){ state = l_press; }
            else if(RIGHT){ state = r_press; }
            else if(FLIP){ state = flip_press; }
            else{
                state = wait;
            }
            break;
        case l_press:
            if(LEFT){ state = l_press; }
            else{ state = l_release; }
            break;
        case l_release:
            state = wait;
            break;
        case r_press:
            if(RIGHT){ state = r_press; }
            else{ state = r_release; }
            break;
        case r_release:
            state = wait;
            break;
        case flip_press:
            if(FLIP){ state = flip_press; }
            else{ state = flip_release; }
            break;
        case flip_release:
            state = wait;
            break;
    }
    switch(state){
        case wait:
            break;
        case l_press:
            break;
        case l_release:
            shift_left();
            break;
        case r_press:
            break;
        case r_release:
            shift_right();
            break;
        case flip_press:
            break;
        case flip_release:
            flip_block();
            break;
    }
    return state;
}

// State machine that outputs values to the LED matrix and score array
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
    PORTB = pt_val;
    PORTC = pattern;	// Pattern to display
	PORTD = row;		// Row(s) displaying pattern
	return state;
}

// Main function with task scheduler
int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;	
    /* Insert your solution below */
	task tasks[3];
	tasks[0].state = s0;
	tasks[0].period = 2;
	tasks[0].elapsedTime = 2;
	tasks[0].TickFct = &disp_tick;

    tasks[1].state = grav;
	tasks[1].period = 1000;
	tasks[1].elapsedTime = 1000;
	tasks[1].TickFct = &tick_grav;

    tasks[2].state = wait;
	tasks[2].period = 10;
	tasks[2].elapsedTime = 10;
	tasks[2].TickFct = &input_tick;

	TimerSet(1);
	TimerOn();
	unsigned short i;
    while(1) {
	    for(i=0; i < 3; i++){
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
