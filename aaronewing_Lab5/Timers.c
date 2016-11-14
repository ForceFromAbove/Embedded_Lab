/*
 * Timer.c
 *
 *  Created on: Nov 9, 2016
 *      Author: aaronewing
 */

#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>
#include "Timers.h"

// timer for "random" (continuous)
void initialize_TimerA0(void) {
//	TA0CCTL0 = CCIE;                          	// CCR0 interrupt enabled
//	TA0CCR0 = 32768;							// count up to 1 second with ACLK
//	TA0CTL = TASSEL_0 + MC_2 + TACLR + TAIE;   	// ACLK, continuous mode, clear TAR, enable TA interrupt

	TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
	TA1CCR0 = 0xFFFF;
	TA1CTL = TASSEL_0 + MC_2 + TACLR;         // SMCLK, contmode, clear TAR
}

// timer for LED (up)
void initialize_TimerB0(void) {
	TB0CCTL0 = CCIE;                          	// CCR0 interrupt enabled
	TB0CCR0 = (32768 + TA0R);					// count up to 1 second + whatever TimerA1 counted to before switch 1 was pressed
	TB0CTL = TASSEL_0 + MC_1 + TACLR + TBIE;         	// ACLK, up mode, clear TAR, enable TB interrupt

	//when joystick is pressed, do TB0CTL = MC_0; to pause, and then grab number.
}

// timer for getting reaction time (up)
void initialize_TimerA1(void) {
	TA1CCTL0 = CCIE;                          	// CCR0 interrupt enabled
	TA1CCR0 = 0xFFFF;							// count up to 2 second (enough time for reaction timer)
	TA1CTL = TASSEL_0 + MC_1 + TACLR;         	// ACLK, up mode, clear TAR
}
