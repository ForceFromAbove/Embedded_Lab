/*
 * Initialize.h
 *
 *  Created on: Nov 11, 2016
 *      Author: aaronewing
 */

// contains all functions for Initializing MSP430

#ifndef INITIALIZE_H_
#define INITIALIZE_H_

void initialize_Joystick(void);
void initialize_Ports(void);
void initialize_Clocks(void);
void initialize_LED(void);
void initialize_Switches(void);
void initialize_UART(void);
void initialize_Interrupts(void);

#endif /* INITIALIZE_H_ */
