#include <msp430.h> 
#include <stdbool.h>
#include <stdint.h>

#define LED1 BIT0
#define LED2 BIT1

#define LEFT 0xFD
#define RIGHT 0xFB
#define CENTER 0xF7
#define UP 0xEF
#define DOWN 0xDF
#define no_Input 0xFF


void led_Blink(led_1or2) {
	if (led_1or2 == 0) {
		P1OUT |= LED1;			// Blink LED1
		__delay_cycles(2000);
		P1OUT &= ~LED1;
	} else {
		P1OUT |= LED2;			// Blink LED2
		__delay_cycles(2000);
		P1OUT &= ~LED2;
	}
}

enum LA_States { LA_SMStart, wait_To_Start, start_Experiment, LED, joystick_Input, UART_Write } LA_State;
void TickFct_Latch() {
  switch(LA_State) {   // Transitions
  	 default:
     case LA_SMStart:  // Initial transition
        LA_State = wait_to_Start;
        break;

	case wait_To_Start:
		if (P2IN == BIT6) {						// if Switch 1 is pressed
			LA_State = start_Experiment;
		} else {
			// stay
		}
		break;

     case start_Experiment:
		LA_State = LED;
		break;

	case LED:
		if (P2IN &= ~RIGHT) {
			LA_State = joystick_Input;
		} else {
			// stay
		}
		break;

	case joystick_Input:
		if (P2IN &= ~RIGHT) {
			LA_State = four_Correct;
		}
		if (P2IN &= ~(LEFT | CENTER | UP | DOWN)) {
			LA_State = zero_Correct;
		} else {
			LA_State = three_Correct;
		}
		break;

	case UART_Write:
		if (P2IN &= ~RIGHT) {
			LA_State = all_Correct;
		}
		if (P2IN &= ~(LEFT | CENTER | UP | DOWN)) {
			LA_State = zero_Correct;
		} else {
			LA_State = four_Correct;
		}
		break;

  } // Transitions

	switch (LA_State) {   // State actions
		case zero_Correct:
			P1OUT &= ~LED2;
			P1OUT &= ~LED1;
			break;

		case one_Correct:
			led_Blink(0);
			break;

		case two_Correct:
			led_Blink(0);
			break;

		case three_Correct:
			led_Blink(0);
			break;

		case four_Correct:
			led_Blink(0);
		break;

		case all_Correct:
			P1OUT |= LED2;
			led_Blink(0);
			break;

		default:
			break;
	} // State actions
}

// NOTES: try putting as |= instead of &= ~ for all inputs

// main.c <Code is UP DOWN LEFT RIGHT RIGHT>
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    bool led_1or2 = 0;			// init var, LEDs, joysticks, and buttons
    led_Init();
    joystick_Init();
    button_Init();

    initialize_Joystick();		// initialize joystick
    initialize_Clocks();		// initialize clocks
    initialize_LED();			// initialize LEDs
    initialize_Switches();		// initialize switches
    initalize_Ports();
    initialize_UART();			// initialize UART connection (for PC output)

    while (1) {					// run state machine
    	TickFct_Latch();
    }

    // The LOCK is UP UP DOWN DOWN LEFT RIGHT LEFT RIGHT
	
	while (1) {
//		switch (P2OUT) {
		if (P2OUT == BIT1) {	// LEFT
			led_Blink(0);
		}
		if (P2OUT == BIT2) {	// RIGHT
			led_Blink(0);
		}
		if (P2OUT == BIT3) {	// CENTER
			led_Blink(0);
		}
		if (P2OUT == BIT4) {	// UP
			led_Blink(0);
			while (P2OUT &= ~BIT4) {
				if (P2OUT == BIT4) {	// UP again
					led_Blink(0);
					while (P2OUT &= ~BIT4) {
						if (P2OUT == BIT1) {	// LEFT
						led_Blink(0);
						while (P2OUT &= ~BIT1) {
							if (P2OUT == BIT2) {	// RIGHT again
							led_Blink(0);
							while (P2OUT &= ~BIT2) {
								if (P2OUT == BIT4) {	// LEFT again
									P1OUT |= BIT1;
									led_Blink(0);
								}
							}
							}
						}
						}
					}
				}
			}
		}
		if (P2OUT == BIT5) {	// DOWN
			led_Blink(0);
		} else {
			led_Blink(0);
		}
//	}
	}
}

