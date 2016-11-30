/* Aaron Ewing - the unholy child of lab 2 and lab 4 (AKA: triggered state machine)
 *
 * Lab 2 is for joystick lab, lab 4 is the UART lab
 *
 * Task Order:
 * 		digital lock SM
 * 		character capitalization SM
 *		programming mode SM.
 *
 * 50 ms clock based off 1 MHz clock
 *
 *
 */




#include <msp430.h> 
#include "Initialize.h"
#include "UART.h"

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

////////////////////////////////////// task struct /////////////////////////////////////////////////

typedef struct task {
   signed   char state;
   unsigned long period;
   unsigned long elapsedTime;
   unsigned char active; // 1: active, 0: inactive
   int (*TickFct)(int);
} task;

task tasks[2]; // Global so visible to tick fct and ISR
const unsigned short tasksNum = 2;

const unsigned long BL_period = 1500;
const unsigned long TLT_period = 500;
const unsigned long tasksPeriodGCD = 500;

enum BL_States { BL_SMStart, BL_LEDOff, BL_LEDOn };
int TickFct_BlinkLed(int state);

enum TLT_States { TLT_SMStart, TLT_T0, TLT_T1, TLT_T2, TLT_Init, TLT_Inactive };
int TickFct_ThreeLedsTriggered(int state);

unsigned char processingRdyTasks = 0;
void TimerISR() {
   unsigned char i;
   if (processingRdyTasks) {
      printf("Timer ticked before task processing done.\r\n");
   }
   else { // Heart of the scheduler code
      processingRdyTasks = 1;
      for (i=0; i < tasksNum; ++i) {
         if (tasks[i].elapsedTime >= tasks[i].period && tasks[i].active) {
         // Ready
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            tasks[i].elapsedTime = 0;
         }
         if (tasks[i].active) { tasks[i].elapsedTime += tasksPeriodGCD; }
      }
      processingRdyTasks = 0;
   }
}


volatile unsigned char RxFlag=0;
volatile unsigned char RxData=0;

const unsigned int tasksTLTIndex = 1; // Reference the TLT task in RxISR and TickFct_ThreeLedsTriggered

void RxISR(void) {
   tasks[tasksTLTIndex].active = 1; // TLT triggered by UART rx; make active
   tasks[tasksTLTIndex].elapsedTime = tasks[tasksTLTIndex].period; // Make ready to tick
   RxData = R; // Read to global var to avoid problems if chars come too fast
   RxFlag = 1;
}

// Task tick functions

////////////////////////////////////// Lab 2 ///////////////////////////////////////////////////////
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
////////////////////////////////////// Lab 2 ///////////////////////////////////////////////////////

////////////////////////////////////// Lab 4 ///////////////////////////////////////////////////////
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
////////////////////////////////////// Lab 4 ///////////////////////////////////////////////////////

////////////////////////////////////// main ////////////////////////////////////////////////////////


void main() {
   unsigned char i=0;
   tasks[i].state       = BL_SMStart;
   tasks[i].period      = BL_period;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].active      = 1; // Task starts as active
   tasks[i].TickFct     = &TickFct_BlinkLed;
   i++;
   tasks[i].state       = TLT_SMStart;
   tasks[i].period      = TLT_period;
   tasks[i].elapsedTime = tasks[i].period;
   tasks[i].active      = 1; // Task starts as active
   tasks[i].TickFct     = &TickFct_ThreeLedsTriggered;

   TimerSet(tasksPeriodGCD);
   TimerOn();
   UARTOn();

   while(1) {
	   Sleep();
   }
}
