/*
 * Time_Conversion.c
 *
 *  Created on: Nov 17, 2016
 *      Author: AaronEwing
 */

#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>

uint16_t Time_Conversion (uint16_t reaction_Time) {	// gets clock cycles and returns milliseconds
	uint16_t milliseconds = 0;
	uint16_t work = 0;

	work = reaction_Time*1000;		// go from seconds to milliseconds
	milliseconds = work/32768;

	return milliseconds;
}


