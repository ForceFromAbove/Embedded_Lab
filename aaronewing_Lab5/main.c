#include <msp430.h> 
#include <stdbool.h>
#include <stdint.h>

#include "Initialize.h"
#include "Timers.h"
#include "UART.h"

bool joystick_Flag = 0;
bool switch_Flag = 0;
bool TimerA0_Flag = 1;
bool TimerA1_Flag = 0;
bool TimerB0_Flag = 0;
bool UART_Flag = 0;

uint16_t random_Time = 0;
uint32_t reaction_Time = 0;
uint32_t reaction_Time2 = 0;
uint16_t milliseconds = 0;

enum Timer_States { random_Timer, LED_Timer, reaction_Timer } Timer_State;
void TickFct_Timer() {
	switch(Timer_State) {   // Transitions
  		case random_Timer:  			// Run random timer
  			if (TimerA0_Flag) {
  				// stay
  				Timer_State = random_Timer;
  			} else {
  				Timer_State = LED_Timer;
  			}
  			break;

  		case LED_Timer:
  			if (TimerB0_Flag) {								// if random timer goes off (B0)
  			Timer_State = reaction_Timer;
			} else {
				// stay
			}
			break;

		case reaction_Timer:
			// do nothing go to interrupt 					timer A0 interrupt to turn on LED
			Timer_State = random_Timer;
			break;

		default:
			break;
	}

/*	switch (Timer_State) {   // State actions
		case random_Timer:
			// do nothing
			break;

		case LED_Timer:
			P1OUT |= BIT0;
			break;

		case reaction_Timer:
			write_UART(reaction_Time, 0);		// send reaction time through UART
			break;

		default:
			break;
	} // State actions */
}

enum LA_States { wait_For_Start, reaction, UART_Transmission } LA_State;
void TickFct_Latch() {
	switch(LA_State) {   // Transitions
		default:
		case wait_For_Start:  		// Wait for SW1
			 if (switch_Flag) {
				LA_State = reaction;
			 } else {
				 // do nothing
			 }
			 break;

		case reaction:
			if (TimerA1_Flag) {						// if 3rd timer tripped
				LA_State = UART_Transmission;
			} else {
				// stay
			}
			break;

		case UART_Transmission:
			if (!TimerA1_Flag) {
			// do nothing go to interrupt 					when done with UART
				LA_State = wait_For_Start;
			} else {
				// stay
			}
			break;
	}

/*	switch (LA_State) {   // State actions
		default:
		case wait_For_Start:
			// wait for switch
			break;

		case reaction:
			// wait for user reaction
			break;

		case UART_Transmission:
			write_UART(reaction_Time, 0);		// send reaction time through UART
			TimerA1_Flag = 0;					// restart reaction timer
			break;
	} // State actions */
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
	case 14:									// P2.6
		//start timer
		initialize_TimerB0();			// initialize timer for A2
		TA0CTL = MC_0;					// pause random timer
		switch_Flag = 1;				// go to Start_Experiment
		TimerA0_Flag = 0;				// TimerA0 is off
	default:
		break;
	}
}

// Timer_B0 Interrupt Vector (TBIV) handler for LED timer
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMERB0_ISR(void) {

	TA0CTL = MC_0;					// pause A0 timer
	TB0CTL = MC_0;					// pause B0 timer
	P1OUT &= ~BIT1;					// turn off LED2
	P1OUT |= BIT0;					// turn on LED1
	initialize_TimerA1();			// start capture timer
}

// Timer A0 interrupt
// service routine for random timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMERA0_ISR(void) {
  P1OUT ^= 0x02;                            // Toggle LED2
}


// Timer A1 interrupt service routine for random timer
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMERA1_ISR(void) {
	TA1CTL = MC_0;							// pause A1 timer
	reaction_Time = TA1R;					// capture the reaction time
	TimerA1_Flag = 1;						// timer A1 is done
	P1OUT &= ~BIT0;							// turn off LED1
	reaction_Time2 = (reaction_Time*1000);
	reaction_Time = reaction_Time2/32768;	// reaction time in seconds
	UART_Flag = 1;							// go to UART section
	write_UART(reaction_Time, 0);			// send reaction time through UART
}
