/*      Author: Justin Wang
 *      Partner(s) Name: 
 *      Lab Section:
 *      Assignment: Lab #11  Exercise #2
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
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

unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a % b;
		if( c == 0 ) { return b; }
		a = b;
		b = c;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
//Struct for Tasks represent a running process in our simple real-time operating system
typedef struct task{
	// Tasks should have members that include: state, period,
	//a measurement of elapsed time, and a function pointer.
	signed 	 char state; 		//Task's current state
	unsigned long period; 		//Task period
	unsigned long elapsedTime; 	//Time elapsed since last task tick
	int (*TickFct)(int); 		//Task tick function
} task;

unsigned char SetBit(unsigned char pin, unsigned char number, unsigned char bin_value) 
{
	return (bin_value ? pin | (0x01 << number) : pin & ~(0x01 << number));
}

////////////////////////////////////////////////////////////////////////////////
//Functionality - Gets bit from a PINx
//Parameter: Takes in a uChar for a PINx and the pin number
//Returns: The value of the PINx
unsigned char GetBit(unsigned char port, unsigned char number) 
{
	return ( port & (0x01 << number) );
}

// Keypad Setup Values
#define KEYPADPORT PORTC
#define KEYPADPIN  PINC
#define ROW1 0
#define ROW2 1
#define ROW3 2
#define ROW4 3
#define COL1 4
#define COL2 5
#define COL3 6
#define COL4 7

////////////////////////////////////////////////////////////////////////////////
//Functionality - Gets input from a keypad via time-multiplexing
//Parameter: None
//Returns: A keypad button press else '\0'
unsigned char GetKeypadKey() {
    unsigned char tmpVal;
    unsigned char num_pressed = 0x00;
	// Check keys in col 1
	KEYPADPORT = SetBit(0xFF,COL1,0); // Set Px4 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { tmpVal = '1'; num_pressed += 1; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { tmpVal = '4'; num_pressed += 1; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { tmpVal = '7'; num_pressed += 1; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { tmpVal = '*'; num_pressed += 1; }

	// Check keys in col 2
	KEYPADPORT = SetBit(0xFF,COL2,0); // Set Px5 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { tmpVal = '2'; num_pressed += 1; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { tmpVal = '5'; num_pressed += 1; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { tmpVal = '8'; num_pressed += 1; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { tmpVal = '0'; num_pressed += 1; }

	// Check keys in col 3
	KEYPADPORT = SetBit(0xFF,COL3,0); // Set Px6 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if ( GetBit(~KEYPADPIN,ROW1) ) { tmpVal = '3'; num_pressed += 1; }
	if ( GetBit(~KEYPADPIN,ROW2) ) { tmpVal = '6'; num_pressed += 1; }
	if ( GetBit(~KEYPADPIN,ROW3) ) { tmpVal = '9'; num_pressed += 1; }
	if ( GetBit(~KEYPADPIN,ROW4) ) { tmpVal = '#'; num_pressed += 1; }

	// Check keys in col 4
	KEYPADPORT = SetBit(0xFF,COL4,0); // Set Px7 to 0; others 1
	asm("nop"); // add a delay to allow PORTx to stabilize before checking
	if (GetBit(~KEYPADPIN,ROW1) ) { tmpVal = 'A'; num_pressed += 1; }
	if (GetBit(~KEYPADPIN,ROW2) ) { tmpVal = 'B'; num_pressed += 1; }
	if (GetBit(~KEYPADPIN,ROW3) ) { tmpVal = 'C'; num_pressed += 1; }
	if (GetBit(~KEYPADPIN,ROW4) ) { tmpVal = 'D'; num_pressed += 1; }
	
    if(num_pressed == 0){
        tmpVal = '\0';
    }
    if(num_pressed > 1){
        tmpVal = 'E';
    }
	return tmpVal;
}

#define B7 (~PINB & 0x80)

enum states { start, w1, u1, w2, u2, w3, u3, w4, u4, w5, u5, w6, u6, unlocked };

int tick(int state) {
    unsigned char key;
    key = GetKeypadKey();
    switch(state){
        case start:
            state = w1;
            break;
        case w1:
            if(key == '#'){ state = u1; }
            else{ state = w1; }
            break;
        case u1:
            if(key == '#'){ state = u1; }
            else if (key == '\0'){ state = w2; }
            else{ state = w1; }
            break;
        case w2:
            if(key == '1'){ state = u2; }
            else if(key == '#'){ state = u1; }
            else if (key == '\0'){ state = w2; }
            else{ state = w1; }
            break;
        case u2:
            if(key == '1'){ state = u2; }
            else if (key == '\0'){ state = w3; }
            else{ state = w1; }
            break;
        case w3:
            if(key == '2'){ state = u3; }
            else if(key == '#'){ state = u1; }
            else if (key == '\0'){ state = w3; }
            else{ state = w1; }
            break;
        case u3:
            if(key == '2'){ state = u3; }
            else if (key == '\0'){ state = w4; }
            else{ state = w1; }
            break;
        case w4:
            if(key == '3'){ state = u4; }
            else if(key == '#'){ state = u1; }
            else if (key == '\0'){ state = w4; }
            else{ state = w1; }
            break;
        case u4:
            if(key == '3'){ state = u4; }
            else if (key == '\0'){ state = w5; }
            else{ state = w1; }
            break;
        case w5:
            if(key == '4'){ state = u5; }
            else if(key == '#'){ state = u1; }
            else if (key == '\0'){ state = w5; }
            else{ state = w1; }
            break;
        case u5:
            if(key == '4'){ state = u5; }
            else if (key == '\0'){ state = w6; }
            else{ state = w1; }
            break;
        case w6:
            if(key == '5'){ state = u6; }
            else if(key == '#'){ state = u1; }
            else if (key == '\0'){ state = w6; }
            else{ state = w1; }
            break;
        case u6:
            if(key == '5'){ state = u6; }
            else if (key == '\0'){ state = unlocked; }
            else{ state = w1; }
            break;
        case unlocked:
            if(B7){
                state = w1;
            }
            else{ state = unlocked; }
            break;
    }

    switch(state){
        case start:
            PORTB = PORTB & 0xFE;
            break;
        case w1:
            PORTB = PORTB & 0xFE;
            break;
        case u1:
            break;
        case w2:
            break;
        case u2:
            break;
        case w3:
            break;
        case u3:
            break;
        case w4:
            break;
        case u4:
            break;
        case w5:
            break;
        case u5:
            break;
        case w6:
            break;
        case u6:
            break;
        case unlocked:
            PORTB = PORTB | 0x01;
            break; 
        default:
            break;
    }
    return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0x0F; PORTB = 0xF0;
    DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xF0; PORTC = 0x0F;
    
    task tasks[1];
    const unsigned short numTasks = 1;

    tasks[0].state = start;
    tasks[0].period = 2;
    tasks[0].elapsedTime = 2;
    tasks[0].TickFct = &tick;

    TimerSet(2);
    TimerOn();
    
    unsigned short i;
    while (1) {
        for(i=0; i< numTasks; i++){
            if(tasks[i].elapsedTime >= tasks[i].period){
                tasks[i].state = tasks[i].TickFct(tasks[i].state);
                tasks[i].elapsedTime = 0;
            }
            tasks[i].elapsedTime += 2;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 0;
}

