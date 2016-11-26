#include <msp430.h> 
#include <stdbool.h>
#include <stdint.h>

#define LEFT 0xFD
#define RIGHT 0xFB
#define CENTER 0xF7
#define UP 0xEF
#define DOWN 0xDF
#define no_Input 0xFF

bool LEFT_Flag = 0;
bool RIGHT_Flag = 0;
bool CENTER_Flag = 0;
bool UP_Flag = 0;
bool DOWN_Flag = 0;
bool SW2_Flag = 0;

uint8_t entry_1 = UP;
uint8_t entry_2 = DOWN;
uint8_t entry_3 = LEFT;
uint8_t entry_4 = RIGHT;
uint8_t entry_5 = CENTER;
uint8_t joystick_Input = 0;

uint8_t i;
bool interrupt_Flag = 0;

void led_Init(void) {
	P1DIR |= BIT0 | BIT1;		// Sets P1.0 and P1.1 as output (LED1 and LED2)
	P1OUT &= ~(BIT0 | BIT1);	// Turns LEDs off
}

void led_Blink(led_1or2) {
	if (led_1or2 == 0) {
		P1OUT |= BIT0;			// Blink LED1
		__delay_cycles(10000);
		P1OUT &= ~BIT0;
	} else {
		P1OUT |= BIT1;			// Blink LED2
		__delay_cycles(10000);
		P1OUT &= ~BIT1;
	}
}

void joystick_Init(void) {
		P2DIR &= ~(LEFT | RIGHT | CENTER | UP | DOWN);	// Sets up joystick as input
					// P2.1 - LEFT, P2.2 - RIGHT, P2.3 - CENTER, P2.4 - UP, DOWN - P2.5
					// 0 if pushed, 1 if not.
		P2REN |= LEFT | RIGHT | CENTER | UP | DOWN;
		P2OUT |= LEFT | RIGHT | CENTER | UP | DOWN;
}

void button_Init(void) {
	P2DIR &= ~(BIT6 | BIT7);	// Init P2.6 and P2.7 as inputs
}

enum LA_States { LA_SMStart, zero_Correct, one_Correct, two_Correct, three_Correct, four_Correct, all_Correct, set_Lock } LA_State;
void TickFct_Latch() {
  switch(LA_State) {   // Transitions
     case LA_SMStart:  // Initial transition
        LA_State = zero_Correct;
        break;

	case zero_Correct:
		if (entry_1 == joystick_Input) {
			LA_State = one_Correct;
		} if (SW2_Flag) {
//			__delay_cycles(2000000);		// wait 2 seconds
			if (SW2_Flag) {
				LA_State = set_Lock;
			}
		} else {
			// stay
		}
		break;

     case one_Correct:
		if (entry_2 == joystick_Input) {		// correct
			LA_State = two_Correct;
			break;
		} if (SW2_Flag) {
			__delay_cycles(2000000);		// wait 2 seconds
			if (SW2_Flag) {
				LA_State = set_Lock;
			}
		} if (P2IN == no_Input) {
			LA_State = one_Correct;
		}
    	else {
			LA_State = zero_Correct;
		}
		break;

	case two_Correct:
		if (entry_3 == joystick_Input) {					// correct
			LA_State = three_Correct;
			break;
		} if (SW2_Flag) {
			__delay_cycles(2000000);		// wait 2 seconds
			if (SW2_Flag) {
				LA_State = set_Lock;
			}
		} if (P2IN == no_Input) {
			LA_State = two_Correct;
		} else {
			LA_State = zero_Correct;
		}
		break;

	case three_Correct:
		if (entry_4 == joystick_Input) {					// correct
			LA_State = four_Correct;
			break;
		} if (SW2_Flag) {
			__delay_cycles(2000000);		// wait 2 seconds
			if (SW2_Flag) {
				LA_State = set_Lock;
			}
		} if (P2IN == no_Input) {
			LA_State = three_Correct;
		} else {
			LA_State = zero_Correct;
		}
		break;

	case four_Correct:
		if (entry_5 == joystick_Input) {					// correct
			LA_State = all_Correct;
			break;
		} if (SW2_Flag) {
			__delay_cycles(2000000);		// wait 2 seconds
			if (SW2_Flag) {
				LA_State = set_Lock;
			}
		} if (P2IN == no_Input) {
			LA_State = four_Correct;
		} else {
			LA_State = zero_Correct;
		}
		break;

	case all_Correct:
		if (P2IN != no_Input) {
			LA_State = zero_Correct;
		} if (SW2_Flag) {
			__delay_cycles(2000000);		// wait 2 seconds
			if (SW2_Flag) {
				LA_State = set_Lock;
			}
		} else {
			LA_State = all_Correct;
		}
		break;

	case set_Lock:
		LA_State = zero_Correct;

     default:
        LA_State = LA_SMStart;
        break;
  } // Transitions

	switch (LA_State) {   // State actions
		case zero_Correct:
			P1OUT &= ~(BIT0 + BIT1);
			break;

		case one_Correct:
//			led_Blink(0);
			break;

		case two_Correct:
//			led_Blink(0);
			break;

		case three_Correct:
//			led_Blink(0);
			break;

		case four_Correct:
//			led_Blink(0);
		break;

		case all_Correct:
			P1OUT |= BIT1;
//			__delay_cycles(100000);
			break;

		case set_Lock:
			while (interrupt_Flag == 0) {};		// set user input when interrupt occured
			entry_1 = joystick_Input;
			interrupt_Flag = 0;

			while (interrupt_Flag == 0) {};		// set user input when interrupt occured
			entry_2 = joystick_Input;
			interrupt_Flag = 0;

			while (interrupt_Flag == 0) {};		// set user input when interrupt occured
			entry_3 = joystick_Input;
			interrupt_Flag = 0;

			while (interrupt_Flag == 0) {};		// set user input when interrupt occured
			entry_4 = joystick_Input;
			interrupt_Flag = 0;

			while (interrupt_Flag == 0) {};		// set user input when interrupt occured
			entry_5 = joystick_Input;
			interrupt_Flag = 0;

		default:
			break;
	} // State actions
}
// Code is UP DOWN LEFT RIGHT CENTER
void main() {
//   B = 0x00;              // Initialize outputs
	led_Init();
	joystick_Init();
	button_Init();

	LA_State = LA_SMStart; // Indicates initial call

	P2IE |= BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT7;		// enable interrupts for joystick left, right, center, up, down, and switch 2
	P2IES |= BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT7;	// interrupt on high-to-low transition
	__bis_SR_register(GIE);       // enable interrupt

	while(1) {
		TickFct_Latch();
	}
}

// Port 2 interrupt service routine for Switch 2/joystick
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {

	if (LA_State == set_Lock) {
		interrupt_Flag = 1;					// there was an interrput
	}

	switch (__even_in_range(P2IV, 14)) {
	case 2: 							// P2.0
		break;
	case 4:								// P2.1
		LEFT_Flag = 1;
		joystick_Input = LEFT;

		RIGHT_Flag = 0;
		CENTER_Flag = 0;
		UP_Flag = 0;
		DOWN_Flag = 0;
		SW2_Flag = 0;

//		led_Blink(0);

		break;

	case 6:								// P2.2
		RIGHT_Flag = 1;
		joystick_Input = RIGHT;

		LEFT_Flag = 0;
		CENTER_Flag = 0;
		UP_Flag = 0;
		DOWN_Flag = 0;
		SW2_Flag = 0;

//		led_Blink(0);

		break;

	case 8:								// P2.3
		CENTER_Flag = 1;
		joystick_Input = CENTER;

		LEFT_Flag = 0;
		RIGHT_Flag = 0;
		UP_Flag = 0;
		DOWN_Flag = 0;
		SW2_Flag = 0;

//		led_Blink(0);

		break;

	case 10:							// P2.4
		UP_Flag = 1;
		joystick_Input = UP;

		LEFT_Flag = 0;
		RIGHT_Flag = 0;
		CENTER_Flag = 0;
		DOWN_Flag = 0;
		SW2_Flag = 0;

//		led_Blink(0);

		break;

	case 12:							// P2.5
		DOWN_Flag = 1;
		joystick_Input = DOWN;

		LEFT_Flag = 0;
		RIGHT_Flag = 0;
		CENTER_Flag = 0;
		UP_Flag = 0;
		SW2_Flag = 0;

//		led_Blink(0);

		break;

	case 14:							// P2.6
		break;
	case 16:							// P2.7
		SW2_Flag = 1;

		LEFT_Flag = 0;
		RIGHT_Flag = 0;
		CENTER_Flag = 0;
		UP_Flag = 0;
		DOWN_Flag = 0;
		break;

	default:
		break;
	}
}

