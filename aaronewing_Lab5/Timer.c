/*
 * Timer.c
 *
 *  Created on: Nov 9, 2016
 *      Author: aaronewing
 */

// timer for "random" (continuous)
void initialize_TimerA1(void) {
	TA1CCTL0 = CCIE;                          	// CCR0 interrupt enabled
	TA1CCR0 = 32768;							// count up to 1 second with ACLK
	TA1CTL = TASSEL_0 + MC_2 + TACLR;         	// ACLK, continuous mode, clear TAR
}

// timer for LED (up)
void initialize_TimerA0(void) {
	TA0CCTL0 = CCIE;                          	// CCR0 interrupt enabled
	TA0CCR0 = (32768 + TA1R);					// count up to 1 second + whatever TimerA1 counted to before switch 1 was pressed
	TA0CTL = TASSEL_0 + MC_1 + TACLR;         	// ACLK, up mode, clear TAR
}

// timer for getting reaction time (up)
void initialize_TimerB0(void) {
	TB0CCTL0 = CCIE;                          	// CCR0 interrupt enabled
	TB0CCR0 = 0xFFFF;							// count up to 2 second (enough time for reaction timer)
	TB0CTL = TASSEL_0 + MC_1 + TACLR;         	// ACLK, up mode, clear TAR
}