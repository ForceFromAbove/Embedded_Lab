#include <msp430.h> 
#include <stdbool.h>

void led_Init(void) {
	P1DIR |= BIT0 | BIT1;		// Sets P1.0 and P1.1 as output (LED1 and LED2)
	P1OUT &= ~(BIT0 | BIT1);	// Turns LEDs off
}

void led_Blink(led_1or2) {
	if (led_1or2 == 0) {
		P1OUT |= BIT0;			// Blink LED1
		__delay_cycles(2000);
		P1OUT &= ~BIT0;
	} else {
		P1OUT |= BIT1;			// Blink LED2
		__delay_cycles(2000);
		P1OUT &= ~BIT1;
	}

}

void joystick_Init(void) {
	P2DIR &= ~(BIT1 | BIT2 | BIT3 | BIT4 | BIT5);	// Sets up joystick as input
	// P2.1 - LEFT, P2.2 - RIGHT, P2.3 - CENTER, P2.4 - UP, DOWN??
}

void button_Init(void) {
	P2DIR &= ~(BIT6 | BIT7);	// Init P2.6 and P2.7 as inputs
}

// main.c
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    bool led_1or2 = 0;

    led_Init();
    joystick_Init();
    button_Init();

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
