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

void initialize_Clocks(void) {			// Sets all clocks to standard position

	 P11DIR |= 0x07;                           // ACLK, MCLK, SMCLK set out to pins
	 P11SEL |= 0x07;                           // P11.0,1,2 for debugging purposes.

// Initialize LFXT1
	P7SEL |= 0x03;                            // Select XT1
	UCSCTL6 &= ~(XT1OFF);                     // XT1 On
	UCSCTL6 |= XCAP_3;                        // Internal load cap

// Loop until XT1 fault flag is cleared
	do {
		UCSCTL7 &= ~XT1LFOFFG;                  // Clear XT1 fault flags
	} while (UCSCTL7 & XT1LFOFFG);               // Test XT1 fault flag
}

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
	do {
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG); // Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG;// Clear fault flags
	} while (SFRIFG1&OFIFG);                   // Test oscillator fault flag
}

void initialize_Ports(void) {			// sets all pins on all ports as an output (except Port 10)
/*	PM5CTL0 &= ~LOCKLPM5;

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
	P10OUT = 0x000; */
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

void initialize_Interrupts(void) {
	P2IE = BIT1 + BIT2 + BIT6;		// BIT1 = joystick left, BIT2 = joystick right, BIT6 = switch 1

	__bis_SR_register(GIE);       // enable interrupt
}

// old MakerSat code
/*

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
}while (SFRIFG1&OFIFG);                 	  // Test oscillator fault flag /
	CSCTL0_H = 0;                             // Lock CS registers (does not allow change in clock) /

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
*/
