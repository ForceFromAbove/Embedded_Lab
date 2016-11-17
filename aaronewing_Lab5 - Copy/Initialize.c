/*
 * Initialize.c
 *
 *  Created on: Nov 3, 2016
 *      Author: aaronewing
 */

// contains all functions for Initializing MSP430

#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>
#include "Initialize.h"

#define LED1 BIT0
#define LED2 BIT1

#define LEFT 0xFD
#define RIGHT 0xFB
#define CENTER 0xF7
#define UP 0xEF
#define DOWN 0xDF
#define no_Input 0xFF

void initialize_Clocks(void) {			// Sets all clocks to standard position

	 P11DIR |= 0x07;                           // ACLK, MCLK, SMCLK set out to pins
	 P11SEL |= 0x07;                           // P11.0,1,2 for debugging purposes.

// Initialize LFXT1
	P7SEL |= 0x03;                            // Select XT1
	UCSCTL6 &= ~(XT1OFF);                     // XT1 On
	UCSCTL6 |= XCAP_3;                        // Internal load cap

// Loop until XT1 fault flag is cleared
	/*
	do {
		UCSCTL7 &= ~XT1LFOFFG;                  // Clear XT1 fault flags
	} while (UCSCTL7 & XT1LFOFFG);               // Test XT1 fault flag
} */

// Initialize DCO to 16MHz
	__bis_SR_register(SCG0);			// Disable the FLL control loop
	UCSCTL0 = 0x0000;                   // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_3;               	// Set RSELx for DCO = 4.9 MHz
	UCSCTL2 = FLLD_1 + 478;				// Set DCO Multiplier for 16MHz
					   	   	   	   	   	// (N + 1) * FLLRef = Fdco
					   	   	   	   	   // (478 + 1) * 32768 = 15.99MHz
					   	   	   	   	   // Set FLL Div = fDCOCLK/2
	__bic_SR_register(SCG0);           	// Enable the FLL control loop

	// Worst-case settling time for the DCO when the DCO range bits have been
	// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
	// UG for optimization.
	// 32 x 32 x 2.45 MHz / 32,768 Hz = 76563 = MCLK cycles for DCO to settle
	__delay_cycles(76563);

	// Loop until XT1,XT2 & DCO fault flag is cleared
/*	do {
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG); // Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG;// Clear fault flags
	} while (SFRIFG1&OFIFG);                   // Test oscillator fault flag */
}

void initialize_LED(void) {
	P1DIR |= BIT0 | BIT1;		// Sets P1.0 and P1.1 as output (LED1 and LED2)
	P1OUT &= ~(BIT0 | BIT1);	// Turns LEDs off
}

void initialize_Switches(void) {
	P2DIR &= ~(BIT6 | BIT7);	// Init P2.6 and P2.7 as inputs
}

void initialize_Joystick(void) {
	P2DIR &= ~(LEFT | RIGHT | CENTER | UP | DOWN);	// Sets up joystick as input
	// P2.1 - LEFT, P2.2 - RIGHT, P2.3 - CENTER, P2.4 - UP, DOWN - P2.5
	// 0 if pushed, 1 if not.
	P2REN |= LEFT | RIGHT | CENTER | UP | DOWN;
	P2OUT |= LEFT | RIGHT | CENTER | UP | DOWN;
}

void initialize_Interrupts(void) {
	P2IE = BIT1 + BIT2 + BIT6;		// BIT1 = joystick left, BIT2 = joystick right, BIT6 = switch 1

	__bis_SR_register(GIE);       // enable interrupt
}


