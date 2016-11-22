/*
 * UART.c
 *
 *  Created on: Nov 10, 2016
 *      Author: aaronewing
 */

#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>
#include "UART.h"

uint8_t RX_Data = 0;

void initialize_UART(bool baud_Rate, uint8_t pin_Setting) {
	switch (pin_Setting) {
	default:
	case 0:
		// Configure Secondary Function Pins
		P3SEL |= BIT4 | BIT5;					// P3.4 - TX, P3.5 - RX

		// assuming clk is set up already at 16MHz

		// Configure USCI_A0 for SPI operation
		UCA0CTL1 |= UCSWRST;                   // **Put state machine in reset**

		switch (baud_Rate) {
		case 0:
			// Configure Timer for 9600 Baud
			UCA0CTL1 = UCSSEL__ACLK;              // Set ACLK = 32768 as UCBRCLK
			UCA0BR0 = 3;                              // 9600 baud
			UCA0MCTL |= 0x5300;            // 32768/9600 - INT(32768/9600)=0.41
											// UCBRSx value = 0x53 (See UG)
			UCA0BR1 = 0;
			break;

		default:
		case 1:
			// Configure Timer for 38400 Baud
			UCA0CTL1 = UCSSEL__SMCLK;          // Set SMCLK = 1000000 as UCBRCLK
			UCA0BR0 = 0x1A;								// 9600 baud
			UCA0MCTL |= 0x0100;      // 1000000/38400 - INT(1000000/38400)=0.04
									  // UCBRSx value = 0x01 (See UG)
			// N = 0.0529, effectively 38,383.4 Baud
			UCA0BR1 = 0;
			break;
		}
		UCA0CTL1 &= ~UCSWRST; // release from reset                   	 	// **Initialize USCI state machine**

		break;

	case 1:
		// Configure Secondary Function Pins
		P5SEL |= BIT6 | BIT7;					// P5.6 - TX, P5.7 - RX

		// assuming clk is set up already at 16MHz

		// Configure USCI_A0 for SPI operation
		UCA1CTL1 |= UCSWRST;                   // **Put state machine in reset**

		switch (baud_Rate) {
		case 0:
			// Configure Timer for 9600 Baud
			UCA1CTL1 = UCSSEL__ACLK;              // Set ACLK = 32768 as UCBRCLK
			UCA1BR0 = 3;                              // 9600 baud
			UCA1MCTL |= 0x5300;            // 32768/9600 - INT(32768/9600)=0.41
											// UCBRSx value = 0x53 (See UG)
			UCA1BR1 = 0;
			break;

		default:
		case 1:
			// Configure Timer for 38400 Baud
			UCA1CTL1 = UCSSEL__SMCLK;          // Set SMCLK = 1000000 as UCBRCLK
			UCA1BR0 = 0x1A;								// 9600 baud
			UCA1MCTL |= 0x0100;      // 1000000/38400 - INT(1000000/38400)=0.04
									  // UCBRSx value = 0x01 (See UG)
			// N = 0.0529, effectively 38,383.4 Baud
			UCA1BR1 = 0;
			break;
		}
		UCA1CTL1 &= ~UCSWRST; // release from reset                   	 	// **Initialize USCI state machine**
		break;

	case 2:
		// Configure Secondary Function Pins
		P9SEL |= BIT4 | BIT5;					// P9.4 - TX, P9.5 - RX

		// assuming clk is set up already at 16MHz

		// Configure USCI_A0 for SPI operation
		UCA2CTL1 |= UCSWRST;                   // **Put state machine in reset**

		switch (baud_Rate) {
		case 0:
			// Configure Timer for 9600 Baud
			UCA2CTL1 = UCSSEL__ACLK;              // Set ACLK = 32768 as UCBRCLK
			UCA2BR0 = 3;                              // 9600 baud
			UCA2MCTL |= 0x5300;            // 32768/9600 - INT(32768/9600)=0.41
											// UCBRSx value = 0x53 (See UG)
			UCA2BR1 = 0;
			break;

		default:
		case 1:
			// Configure Timer for 38400 Baud
			UCA2CTL1 = UCSSEL__SMCLK;          // Set SMCLK = 1000000 as UCBRCLK
			UCA2BR0 = 0x1A;								// 9600 baud
			UCA2MCTL |= 0x0100;      // 1000000/38400 - INT(1000000/38400)=0.04
									  // UCBRSx value = 0x01 (See UG)
			// N = 0.0529, effectively 38,383.4 Baud
			UCA2BR1 = 0;
			break;
		}
		UCA2CTL1 &= ~UCSWRST; // release from reset                   	 	// **Initialize USCI state machine**
		break;

	case 3:
		// Configure Secondary Function Pins
		P10SEL |= BIT4 | BIT5;					// P10.4 - TX, P10.5 - RX

		// assuming clk is set up already at 16MHz

		// Configure USCI_A0 for SPI operation
		UCA3CTL1 |= UCSWRST;                   // **Put state machine in reset**

		switch (baud_Rate) {
		case 0:
			// Configure Timer for 9600 Baud
			UCA3CTL1 = UCSSEL__ACLK;              // Set ACLK = 32768 as UCBRCLK
			UCA3BR0 = 3;                              // 9600 baud
			UCA3MCTL |= 0x5300;            // 32768/9600 - INT(32768/9600)=0.41
											// UCBRSx value = 0x53 (See UG)
			UCA3BR1 = 0;
			break;

		default:
		case 1:
			// Configure Timer for 38400 Baud
			UCA3CTL1 = UCSSEL__SMCLK;          // Set SMCLK = 1000000 as UCBRCLK
			UCA3BR0 = 0x1A;								// 9600 baud
			UCA3MCTL |= 0x0100;      // 1000000/38400 - INT(1000000/38400)=0.04
									  // UCBRSx value = 0x01 (See UG)
			// N = 0.0529, effectively 38,383.4 Baud
			UCA3BR1 = 0;
			break;
		}
		UCA3CTL1 &= ~UCSWRST; // release from reset                   	 	// **Initialize USCI state machine**
		break;
	}
}

void write_UART(uint32_t TX_Data, uint8_t pin_Setting) {
	switch (pin_Setting) {
	default:
	case 0:
		while (!(UCA0IFG & UCTXIFG)) {};					// If able to TX
		UCA0TXBUF = TX_Data >> 24;							// 8 bits transmitted (1st byte)

		while (!(UCA0IFG & UCTXIFG)) {};					// If able to TX
		UCA0TXBUF = TX_Data >> 16;							// 8 bits transmitted (2nd byte)

		while (!(UCA0IFG & UCTXIFG)) {};					// If able to TX
		UCA0TXBUF = TX_Data >> 8;							// 8 bits transmitted (3rd byte)

		while (!(UCA0IFG & UCTXIFG)) {};					// If able to TX
		UCA0TXBUF = TX_Data;								// 8 bits transmitted (4th byte)
		break;

	case 1:
		while (!(UCA1IFG & UCTXIFG)) {};					// If able to TX
		UCA1TXBUF = TX_Data;							// 8 bits transmitted
		break;

	case 2:
		while (!(UCA2IFG & UCTXIFG)) {};					// If able to TX
		UCA2TXBUF = TX_Data;							// 8 bits transmitted
		break;

	case 3:
		while (!(UCA3IFG & UCTXIFG)) {};					// If able to TX
		UCA3TXBUF = TX_Data;							// 8 bits transmitted
		break;
	}
}

////////////////////// UART READ POLLING //////////////////////////////////
uint8_t read_UART(void) {
	while (!(UCA0IFG & UCRXIFG)) {};    			// While RX flag is high
	RX_Data = UCA0RXBUF;							// Recieve Radio ACK
	return RX_Data;
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
