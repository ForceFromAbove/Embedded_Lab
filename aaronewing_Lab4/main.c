// aaron ewing

#include <msp430.h> 
#include <stdbool.h>
#include <stdint.h>

uint8_t RXData = 0x00;			// global data
uint8_t TXData = 0xFF;			// transmit data
uint8_t state = 0;
bool LED_Flag = 0;				// LED flag

enum UART_States { RX_Data, convert_Data, TX_Data } UART_State;
void TickFct_UART() {
	switch(UART_State) {   // Transitions
  	case RX_Data:
  		if (state == 0) {					// RX state
  			UART_State = RX_Data;
  		}
  		if (state == 1) {					// convert state
  		  	UART_State = convert_Data;
  		}
  		if (state == 2) {					// TX state
  			UART_State = TX_Data;
  		}
  		break;

  	case convert_Data:
		if (state == 0) {					// RX state
			UART_State = RX_Data;
		}
		if (state == 1) {					// convert state
			UART_State = convert_Data;
		}
		if (state == 2) {					// TX state
			UART_State = TX_Data;
		}
		break;

	case TX_Data:
		if (state == 0) {					// RX state
			UART_State = RX_Data;
		}
		if (state == 1) {					// convert state
		  	UART_State = convert_Data;
		}
		if (state == 2) {					// TX state
			UART_State = TX_Data;
		}
		break;

		default:
			break;
	}

	switch (UART_State) {   // State actions
		case RX_Data:
			// do nothing
			break;

		case convert_Data:

			if (RXData >= 0x61 && RXData <= 0x7A) {		// if 'a' to 'z'
				TXData = RXData - 0x20;					// capitalize letter
			} else {
				TXData = RXData;						// do not change input
			}
			state = 2;
			break;

		case TX_Data:
			write_UART(TXData, 0);						// send TXData through UART
			state = 0;
			LED_Flag = 0;								// turn off LED
			break;

		default:
			break;
	} // State actions
}

enum LED_States { no_LED, LED } LED_State;
void TickFct_LED() {
	switch(LED_State) {   // Transitions
	case no_LED:  				// do not turn on LED
		 if (LED_Flag) {
			LED_State = LED;
		 } else {
			 LED_State = no_LED;
		 }
		 break;

	case LED:
	if (LED_Flag) {				// turn on LED
		LED_State = LED;
	} else {
		LED_State = no_LED;
	}
	default:
	break;
	}

	switch (LED_State) {   	// State actions
	default:
	case no_LED:			// turn off LED
		P1OUT &= ~BIT0;
		break;

	case LED:				// turn on LED
		P1OUT |= BIT0;
		break;
	}
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    initialize_LED();			// initialize LEDs
    initialize_UART(0,0);		// initialize UART connection (for PC input/output)
    initialize_Interrupts();	// sets up and enables UART interrupt

    while (1) {					// run state machine
    	TickFct_UART();
    	TickFct_LED();
    }
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void) {
  switch(__even_in_range(UCA0IV,4)) {
  case 0:break;                             // Vector 0 - no interrupt
  case 2:                                   // Vector 2 - RXIFG
    while (!(UCA0IFG & UCTXIFG));           // USCI_A0 TX buffer ready?
    RXData = UCA0RXBUF;                  	// TX -> RXData

    state = 1;								// turn on LED and go to convert state
    LED_Flag = 1;
    break;
  case 4:break;                             // Vector 4 - TXIFG
  default: break;
  }
}
