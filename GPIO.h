/*****************************************************************
 *  GPIO.h
 *
 *  Authors       Kelsey Marquez and Jacob Kucinski
 *  Course        EGR 424: Design of Microcontroller Applications
 *  Instructor    Dr. Parikh
 *  Assignment    Project 2: Slot Machine
 *  Date          6/27/24
 *****************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

/**********PIN MAP************
 * * * * * * * * * * * * * * *
 ******** ST7735 LCD  ********
 * CLK                   >P9.5  green
 * SDA                   >P9.7  grey
 * RS (A0)               >P9.2  orange
 * RST                   >P9.3  purple
 * CS                    >P9.4  blue
 * 3.3V                         white
 * GND                          yellow
 ********** BUTTONS **********
 * STOP                  >P4.6  red
 * START                 >P4.7  orange
******************************/

// Define mask and specific bits for buttons
#define BTN_MASK 0b11000000     // mask for BTN Pins
#define BTN1 BIT6               // define button 1 bit
#define BTN2 BIT7               // define button 2 bit


void pin_init(void);

void PORT4_IRQHandler(void);

#endif /* GPIO_H_ */
