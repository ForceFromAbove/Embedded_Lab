/*
 * Initialize.c
 *
 *  Created on: Nov 3, 2016
 *      Author: aaronewing
 */

// contains all functions for Initializing MSP430

#define LED1 BIT0
#define LED2 BIT1

#define LEFT 0xFD
#define RIGHT 0xFB
#define CENTER 0xF7
#define UP 0xEF
#define DOWN 0xDF
#define no_Input 0xFF

#include <msp430.h>
#include "Initialize.h"

void initialize_Clocks(void) {								// Sets all clocks to standard position

	PJSEL0 |= BIT4 | BIT5;      // For XT1

	// Disable the GPIO power-on default high-impedance mode to activate
	// previously configured port settings


/*	CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers (allows for change in clock)
	CSCTL1 = DCOFSEL_6;                       // Set DCO to 8MHz (option to go to 16 MHz but FRAM doesn't like above 8MHz)
	CSCTL2 = SELA__LFXTCLK | SELS__HFXTCLK | SELM__DCOCLK;
	CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers to 1 (all CLKS at highest frequency)
	CSCTL4 |= LFXTDRIVE_3 | HFXTDRIVE_3;
	CSCTL4 &= ~(LFXTOFF | HFXTOFF);
/*	do
	{
		CSCTL5 &= ~LFXTOFFG;       	// Clear XT1 fault flag
	    SFRIFG1 &= ~OFIFG;
	}while (SFRIFG1&OFIFG);                 	  // Test oscillator fault flag */
		CSCTL0_H = 0;                             // Lock CS registers (does not allow change in clock) */

	// XT1 Setup
	 CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
	CSCTL1 = DCOFSEL_0;                       // Set DCO to 1MHz
		  CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
		  CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // set all dividers
		  CSCTL4 &= ~LFXTOFF;
		  do
		  {
		    CSCTL5 &= ~LFXTOFFG;                    // Clear XT1 fault flag
		    SFRIFG1 &= ~OFIFG;
		  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
		  CSCTL0_H = 0;                             // Lock CS registers
}

void initialize_Ports(void) {			// sets all pins on all ports as an output (except Port 10)
	PM5CTL0 &= ~LOCKLPM5;

	P1DIR |= 0xFFFF;
	P2DIR |= 0xFFFF;
	P3DIR |= 0xFFFF;
	P4DIR |= 0xFFFF;
	P5DIR |= 0xFFFF;
	P6DIR |= 0xFFFF;
	P7DIR |= 0xFFFF;
	P8DIR |= 0xFFFF;
	P9DIR |= 0xFFFF;
	P10DIR |= 0xFFFF;					// Pins 0, 1, and 2 are the only ones to exist on Port 10

	P1OUT = 0x000;						// sets all pins on all ports to a low output (redundant)
	P2OUT = 0x000;
	P3OUT = 0x000;
	P4OUT = 0x000;
	P5OUT = 0x000;
	P6OUT = 0x000;
	P7OUT = 0x000;
	P8OUT = 0x000;
	P9OUT = 0x000;
	P10OUT = 0x000;
}

void initialize_LED(void) {
	P1DIR |= BIT0 | BIT1;		// Sets P1.0 and P1.1 as output (LED1 and LED2)
	P1OUT &= ~(BIT0 | BIT1);	// Turns LEDs off
}

void initialize_Switches(void) {
	P2DIR &= ~(BIT6 | BIT7);	// Init P2.6 and P2.7 as inputs
}

void initialize_UART(void) {

}

void initialize_Joystick(void) {
	P2DIR &= ~(LEFT | RIGHT | CENTER | UP | DOWN);	// Sets up joystick as input
	// P2.1 - LEFT, P2.2 - RIGHT, P2.3 - CENTER, P2.4 - UP, DOWN - P2.5
	// 0 if pushed, 1 if not.
	P2REN |= LEFT | RIGHT | CENTER | UP | DOWN;
	P2OUT |= LEFT | RIGHT | CENTER | UP | DOWN;
}
