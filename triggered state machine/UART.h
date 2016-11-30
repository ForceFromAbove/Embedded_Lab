/*
 * UART.h
 *
 *  Created on: Nov 10, 2016
 *      Author: aaronewing
 */

#ifndef UART_H_
#define UART_H_

void init_UART (bool baud_Rate, bool pin_Setting);		// initalizes UART clk rate and which pins are being used
void write_UART (uint8_t TX_Data, uint8_t pin_Setting);	// writes 8 bits with UART
uint8_t read_UART (void);

#endif /* UART_H_ */
