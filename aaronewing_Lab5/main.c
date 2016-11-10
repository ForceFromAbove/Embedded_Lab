#include <msp430.h> 
#include <stdbool.h>
#include <stdint.h>

#include "Initialize.h"
#include "Timers.h"

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

enum Timer_States { random_Timer, LED_Timer, reaction_Timer } Timer_State;
void TickFct_Timer() {
	switch(Timer_State) {   // Transitions
  		default:
  		case random_Timer:  // Initial transition
  			Timer_State = LED_Timer;
  			break;

  		case LED_Timer:
  			//if (P2IN == BIT6) {						// if Switch 1 is pressed *interrupt*
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
			// run continous timer for rand number
			led_Blink(0);
			break;

		case LED_Timer:
			led_Blink(0);
			break;

		case reaction_Timer:
			led_Blink(0);
			break;
	} // State actions

enum LA_States { wait_For_Start, reaction_Time, UART_Transmission } LA_State;
void TickFct_Latch() {
	switch(LA_State) {   // Transitions
		default:
		case wait_For_Start:  // Initial transition
			// if switch 1
				LA_State = reaction_Time;
			break;

		case reaction_Time:
			//if (P2IN == BIT6) {						// if 3rd timer tripped
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
			led_Blink(0);
			break;

		case reaction_Time:
			led_Blink(0);
			break;

		case UART_Transmission:
			led_Blink(0);
			break;
		} // State actions

/*
enum LA_States { LA_SMStart, wait_To_Start, start_Experiment, LED, joystick_Input, UART_Write } LA_State;
void TickFct_Latch() {
  switch(LA_State) {   // Transitions
  	 default:
     case LA_SMStart:  // Initial transition
        LA_State = wait_to_Start;
        break;

	case wait_To_Start:
		//if (P2IN == BIT6) {						// if Switch 1 is pressed *interrupt*
			LA_State = start_Experiment;
		} else {
			// stay
		}
		break;

     case start_Experiment:
    	 // do nothing go to interrupt 					timer A0 interrupt to turn on LED
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
		default:
		case wait_To_Start:
			// run continous timer for rand number
			led_Blink(0);
			break;

		case start_Experiment:
			led_Blink(0);
			break;

		case LED:
			led_Blink(0);
			break;

		case joystick_Input:
			led_Blink(0);
		break;

		case UART_Write:
			P1OUT |= LED2;
			led_Blink(0);
			break;
	} // State actions
} */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    bool led_1or2 = 0;			// init var, LEDs, joysticks, and buttons
    led_Init();
    joystick_Init();
    button_Init();

    initialize_Joystick();		// initialize joystick
    initialize_Clocks();		// initialize clocks & export to test pins P11.0 to P11.2
    initialize_LED();			// initialize LEDs
    initialize_Switches();		// initialize switches
    initalize_Ports();
    initialize_UART();			// initialize UART connection (for PC output)
    initialize_Interrupts();	// sets up and enables all interrupts
    initialize_TimerA1();		// initialize timer for A1

    while (1) {					// run state machine
    	TickFct_Latch();
    }
}

// Port 2 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
switch(__even_in_range(P2IV,14)) {
	case 4:break;                             	// P2.1
	case 6:break;								// P2.2
	case 14:									// P2.6
		//start timer
		initialize_TimerA0();			// initialize timer for A2
		LA_State = start_Experiment;	// go to Start_Experiment
	default: break;
}
