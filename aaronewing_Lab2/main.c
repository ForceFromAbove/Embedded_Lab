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

bool LEFT_Flag = 0;
bool RIGHT_Flag = 0;
bool CENTER_Flag = 0;
bool UP_Flag = 0;
bool DOWN_Flag = 0;
bool SW2_Flag = 0;

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

    led_Init();
    joystick_Init();
    button_Init();

    // The LOCK is UP DOWN UP DOWN LEFT RIGHT LEFT RIGHT

    while (1) {

    	// if lock is correct

		if (P2IN == 0xEF && counter == 1) { // Up and counter is on 1st entry

			led_Blink(0);
			counter = 2;					// blink LED and move to next entry

			while (P2IN != 0xFF) {};		// wait until no user input

		} if (P2IN == 0xDF && counter == 2) { // Down and counter is on 2nd entry

			led_Blink(0);
			counter = 3;					// blink LED and move to next entry

			while (P2IN != 0xFF) {};		// wait until no user input

		} if (P2IN == 0xEF && counter == 3) { // Up and counter is on 3rd entry

			led_Blink(0);
			counter = 4;					// blink LED and move to next entry

			while (P2IN != 0xFF) {};		// wait until no user input

		} if (P2IN == 0xDF && counter == 4) { // Down and counter is on 4th entry

			led_Blink(0);
			counter = 5;					// blink LED and move to next entry

			while (P2IN != 0xFF) {};		// wait until no user input

		} if (P2IN == 0xFD && counter == 5) { // Left and counter is on 5th entry

			led_Blink(0);
			counter = 6;					// blink LED and move to next entry

			while (P2IN != 0xFF) {};		// wait until no user input

    	} if (P2IN == 0xFB && counter == 6) { // Right and counter is on 6th entry

    		led_Blink(0);
    		counter = 7;					// blink LED and move to next entry

    		while (P2IN != 0xFF) {};		// wait until no user input

    	} if (P2IN == 0xFD && counter == 7) { // Left and counter is on 7th entry

    		led_Blink(0);
    		counter = 8;					// blink LED and move to next entry

    		while (P2IN != 0xFF) {};		// wait until no user input

    	} if (P2IN == 0xFB && counter == 8) { // Right and counter is on 8th entry

    		P1OUT |= BIT1;					// turn on LED 2
    		__delay_cycles(20000);			// keep LED 2 on
    		counter = 1;					// reset counter
    		while (P2IN == 0xFF) {};		// wait for user input on joystick
    		P1OUT &= ~BIT1;					// turn off LED 2

    	// if lock is wrong

    	} if (counter == 1 && !(P2IN == 0xEF || P2IN == 0xFF)) {		// if 1st entry is wrong

    		reset_Lock();			// resets Lock

		} if (counter == 2 && !(P2IN == 0xDF || P2IN == 0xFF)) {		// if 2nd entry is wrong

			reset_Lock();			// resets Lock

    	} if (counter == 3 && !(P2IN == 0xEF || P2IN == 0xFF)) {		// if 3rd entry is wrong

    		reset_Lock();			// resets Lock

    	} if (counter == 4 && !(P2IN == 0xDF || P2IN == 0xFF)) {		// if 4th entry is wrong

    		reset_Lock();			// resets Lock

    	} if (counter == 5 && !(P2IN == 0xFD || P2IN == 0xFF)) {		// if 5th entry is wrong

    		reset_Lock();			// resets Lock

    	} if (counter == 6 && !(P2IN == 0xFB || P2IN == 0xFF)) {		// if 6th entry is wrong

       		reset_Lock();			// resets Lock

    	} if (counter == 7 && !(P2IN == 0xFD || P2IN == 0xFF)) {		// if 7th entry is wrong

    		reset_Lock();			// resets Lock

    	} if (counter == 8 && !(P2IN == 0xFB || P2IN == 0xFF)) {		// if 8th entry is wrong

        	reset_Lock();			// resets Lock
    	}
    }
}

// Port 2 interrupt service routine for Switch 2/joystick
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {
	switch (__even_in_range(P2IV, 14)) {
	case 2: 							// P2.0
		break;
	case 4:								// P2.1
		LEFT_Flag = 1;

		RIGHT_Flag = 0;
		CENTER_Flag = 0;
		UP_Flag = 0;
		DOWN_Flag = 0;
		SW2_Flag = 0;
		break;

	case 6:								// P2.2
		RIGHT_Flag = 1;

		LEFT_Flag = 0;
		CENTER_Flag = 0;
		UP_Flag = 0;
		DOWN_Flag = 0;
		SW2_Flag = 0;
		break;

	case 8:								// P2.3
		CENTER_Flag = 1;

		LEFT_Flag = 0;
		RIGHT_Flag = 0;
		UP_Flag = 0;
		DOWN_Flag = 0;
		SW2_Flag = 0;
		break;

	case 10:							// P2.4
		UP_Flag = 1;

		LEFT_Flag = 0;
		RIGHT_Flag = 0;
		CENTER_Flag = 0;
		DOWN_Flag = 0;
		SW2_Flag = 0;
		break;

	case 12:							// P2.5
		DOWN_Flag = 1;

		LEFT_Flag = 0;
		RIGHT_Flag = 0;
		CENTER_Flag = 0;
		UP_Flag = 0;
		SW2_Flag = 0;
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

