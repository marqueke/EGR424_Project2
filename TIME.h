/*
 * TIME.h
 *
 *  Created on: Jun 26, 2024
 *      Author: 31kav
 */

#ifndef TIME_H_
#define TIME_H_

#include <stdint.h>

void set_clk48MHz(void);                        // clock init for LCD

int debounce(int ms, uint8_t button);

void delay_ms(uint32_t ms);

void sys_init();


#endif /* TIME_H_ */
