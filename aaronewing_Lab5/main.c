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
	// 0 if pushed, 1 if not.
		P2REN |= LEFT | RIGHT | CENTER | UP | DOWN;
		P2OUT |= LEFT | RIGHT | CENTER | UP | DOWN;

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
		if (P2IN == UP) {
			LA_State = one_Correct;
		} else {
			// stay
		}
		break;

     case one_Correct:
    	 if (P2IN == LEFT) {
			LA_State = zero_Correct;
    	 }
    	 if (P2IN == RIGHT) {
			LA_State = zero_Correct;
		 }
    	 if (P2IN == CENTER) {
    	 	LA_State = zero_Correct;
    	 }
    	 if (P2IN == UP) {
    	  	LA_State = zero_Correct;
    	 }
		if (P2IN == DOWN) {					// correct
			LA_State = two_Correct;
			break;
		}
    	 if (P2IN == no_Input) {
			LA_State = one_Correct;
		}
    	else {
			LA_State = zero_Correct;
		}
		break;

	case two_Correct:
		if (P2IN &= ~LEFT) {
			LA_State = three_Correct;
		}
		if (P2IN &= ~(RIGHT | CENTER | UP | DOWN)) {
			LA_State = zero_Correct;
		} else {
			LA_State = two_Correct;
		}
		break;

	case three_Correct:
		if (P2IN &= ~RIGHT) {
			LA_State = four_Correct;
		}
		if (P2IN &= ~(LEFT | CENTER | UP | DOWN)) {
			LA_State = zero_Correct;
		} else {
			LA_State = three_Correct;
		}
		break;

	case four_Correct:
		if (P2IN &= ~RIGHT) {
			LA_State = all_Correct;
		}
		if (P2IN &= ~(LEFT | CENTER | UP | DOWN)) {
			LA_State = zero_Correct;
		} else {
			LA_State = four_Correct;
		}
		break;

	case all_Correct:
		if (P2IN &= ~(LEFT | RIGHT | CENTER | UP | DOWN)) {
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
// Code is UP DOWN LEFT RIGHT RIGHT
/*void main() {
   B = 0x00;              // Initialize outputs
   LA_State = LA_SMStart; // Indicates initial call

   while(1) {
      TickFct_Latch();
   }
}*/



// NOTES: try putting as |= instead of &= ~ for all inputs

// main.c <Code is UP DOWN LEFT RIGHT RIGHT>
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    bool led_1or2 = 0;			// init var, LEDs, joysticks, and buttons
    led_Init();
    joystick_Init();
    button_Init();
    initialize_Clocks();		// initialize clocks

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

