/*****************************************************************
 *  TIME.h
 *
 *  Authors       Kelsey Marquez and Jacob Kucinski
 *  Course        EGR 424: Design of Microcontroller Applications
 *  Instructor    Dr. Parikh
 *  Assignment    Project 2: Slot Machine
 *  Date          6/27/24
 *****************************************************************/

#ifndef TIME_H_
#define TIME_H_

#include <stdint.h>

void set_clk48MHz(void);

int debounce(int ms, uint8_t button);

void delay_ms(uint32_t ms);

void sys_init();


#endif /* TIME_H_ */
