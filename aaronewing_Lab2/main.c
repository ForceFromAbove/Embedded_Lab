#include <msp430.h> 
#include <stdbool.h>

#define LED1 0x00
#define LED2 0x01

#define LEFT 0x01
#define RIGHT 0x02
#define CENTER 0x04
#define UP 0x08
#define DOWN 010

void led_Init(void) {
	P1DIR |= BIT0 | BIT1;		// Sets P1.0 and P1.1 as output (LED1 and LED2)
	P1OUT &= ~(BIT0 | BIT1);	// Turns LEDs off
}

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

void joystick_Init(void) {
	P2DIR &= ~(LEFT | RIGHT | CENTER | UP | DOWN);	// Sets up joystick as input
	// P2.1 - LEFT, P2.2 - RIGHT, P2.3 - CENTER, P2.4 - UP, DOWN - P2.5
}

void button_Init(void) {
	P2DIR &= ~(BIT6 | BIT7);	// Init P2.6 and P2.7 as inputs
}

enum LA_States { LA_SMStart, zero_Correct, one_Correct, two_Correct, three_Correct, four_Correct, all_Correct } LA_State;
void TickFct_Latch() {
  switch(LA_State) {   // Transitions
     case LA_SMStart:  // Initial transition
        LA_State = zero_Correct;
        break;

	case zero_Correct:
		if (P2OUT |= UP) {
			LA_State = one_Correct;
		} else {
			// stay
		}
		break;

     case one_Correct:
		if (P2OUT |= DOWN) {
			LA_State = two_Correct;
		} else {
			LA_State = zero_Correct;
		}
		break;

	case two_Correct:
		if (P2OUT |= LEFT) {
			LA_State = three_Correct;
		} else {
			LA_State = zero_Correct;
		}
		break;

	case three_Correct:
		if (P2OUT |= RIGHT) {
			LA_State = four_Correct;
		} else {
			LA_State = zero_Correct;
		}
		break;

	case four_Correct:
		if (P2OUT |= RIGHT) {
			LA_State = all_Correct;
		} else {
			LA_State = zero_Correct;
		}
		break;

	case all_Correct:
		if (P2OUT &= ~(LEFT | RIGHT | CENTER | UP | DOWN)) {
			LA_State = zero_Correct;
		}
		break;

     default:
        LA_State = LA_SMStart;
        break;
  } // Transitions

	switch (LA_State) {   // State actions
		case zero_Correct:
			P1OUT &= ~LED2;
			led_Blink(0);
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
// Code is UP DOWN LEFT RIGHT RIGHT
/*void main() {
   B = 0x00;              // Initialize outputs
   LA_State = LA_SMStart; // Indicates initial call

   while(1) {
      TickFct_Latch();
   }
}*/

// main.c <Code is UP DOWN LEFT RIGHT RIGHT>
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    bool led_1or2 = 0;			// init var, LEDs, joysticks, and buttons
    led_Init();
    joystick_Init();
    button_Init();

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
