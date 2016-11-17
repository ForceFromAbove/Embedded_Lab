#include <msp430.h> 
#include <stdbool.h>
#include <stdint.h>

#include "Initialize.h"
#include "Timers.h"

bool joystick_Flag = 0;
bool switch_Flag = 0;
bool TimerA0_Flag = 1;
bool TimerA1_Flag = 0;
bool TimerB0_Flag = 0;

uint16_t random_Time = 0;
uint16_t reaction_Time = 0;

enum Timer_States { random_Timer, LED_Timer, reaction_Timer } Timer_State;
void TickFct_Timer() {
	switch(Timer_State) {   // Transitions
  		default:
  		case random_Timer:  // Initial transition
  			if (TimerA0_Flag) {
  				// stay
  				Timer_State = random_Timer;
  			} else {
  				Timer_State = LED_Timer;
  			}
  			break;

  		case LED_Timer:
  			if (TimerB0_Flag) {						// if random timer goes off (B0)
  			Timer_State = reaction_Timer;
			} else {
				// stay
			}
			break;

		case reaction_Timer:
			// do nothing go to interrupt 					timer A0 interrupt to turn on LED
			Timer_State = random_Timer;
			break;


	switch (Timer_State) {   // State actions
		default:
		case random_Timer:
			initialize_TimerA0();		// initialize timer for A1 (continous timer for random number)
			TimerA0_Flag = 0;
			break;

		case LED_Timer:
			led_Blink(0);
			break;

		case reaction_Timer:
			led_Blink(0);
			break;
	} // State actions
	}
}

enum LA_States { wait_For_Start, reaction, UART_Transmission } LA_State;
void TickFct_Latch() {
	switch(LA_State) {   // Transitions
		default:
		case wait_For_Start:  // Initial transition
			 if (switch_Flag) {
				LA_State = reaction;
			 } else {
				 // quack
			 }
			 break;

		case reaction:
			if (TimerB0_Flag) {						// if 3rd timer tripped
				LA_State = UART_Transmission;
			} else {
				// stay
			}
			break;

		case UART_Transmission:
			// do nothing go to interrupt 					when done with UART
			LA_State = wait_For_Start;
			break;


	switch (LA_State) {   // State actions
		default:
		case wait_For_Start:
			// run continous timer for rand number
//			random_Time = TA0R;			// grabs random time from TA0 <might not be needed>
			// do nothing?
			break;

		case reaction:
		//	led_Blink(0);
			break;

		case UART_Transmission:
			// convert time to ms
			// transmit through UART
			led_Blink(0);
			break;
		} // State actions
	}
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    initialize_Joystick();		// initialize joystick
    initialize_Clocks();		// initialize clocks & export to test pins P11.0 to P11.2
    initialize_LED();			// initialize LEDs
    initialize_Switches();		// initialize switches
    initialize_UART(0,0);		// initialize UART connection (for PC output)
    initialize_Interrupts();	// sets up and enables all interrupts
    initialize_TimerA0();		// initialize timer for A1

    while (1) {					// run state machine
    	TickFct_Timer();
    	TickFct_Latch();
    }
}

// Port 2 interrupt service routine for Switch 1/joystick
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {
	switch (__even_in_range(P2IV, 14)) {
	case 4:										// P2.1
	case 6:										// P2.2
		TA1CTL = MC_0;						// pause A1 timer
		TA1R = reaction_Time;				// reaction time in clock cycles
		break;
	case 14:									// P2.6
		//start timer
		initialize_TimerB0();			// initialize timer for A2
		switch_Flag = 1;				// go to Start_Experiment
	default:
		break;
	}
}

// Timer_B0 Interrupt Vector (TBIV) handler for LED timer
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMERB0_ISR(void) {
  /* Any access, read or write, of the TBIV register automatically resets the
  highest "pending" interrupt flag. */
/*  switch( __even_in_range(TBIV,14) ) {
    case  0: break;                          // No interrupt
    case  2:
 //   	TB0CTL = MC_0;						// pause B0 timer
    	break;                          // CCR1 not used
    case  4: break;                          // CCR2 not used
    case  6: break;                          // CCR3 not used
    case  8: break;                          // CCR4 not used
    case 10: break;                          // CCR5 not used
    case 12: break;                          // CCR6 not used
    case 14: P1OUT ^= 0x01;                  // overflow
            break;
    default: break;
  } */

	P1OUT |= BIT1;					// turn on LED1
// enable joystick interrupt
// start A1 timer
	initialize_TimerA1();


}

// Timer A0 interrupt service routine for random timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMERA0_ISR(void) {
  P1OUT ^= 0x02;                            // Toggle LED2
}

// Timer A1 interrupt service routine for random timer
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMERA1_ISR(void) {
  P1OUT ^= 0x02;                            // Toggle LED2
}
