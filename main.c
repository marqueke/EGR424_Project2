/*******************************************************************************************************
 *Authors       Kelsey Marquez and Jacob Kucinski
 *Course        EGR 424: Design of Microcontroller Applications
 *Assignment    Project 2: Slot Machine
 *Date          6/27/24
 *Instructor    Dr. Parikh
 *Description   Interface MSP432 with 128x160 ST7735 LCD
 *Notes:        Includes modified Valvano graphics library for interfacing with the ST7735 LCD.
 *              The purpose of this project is to allow the user to run a slot machine controlled by
 *              a START and STOP pushbutton. The slot machine has 9 unique symbols where if any of
 *              the three rows contain three of the same symbol, then a victory message is displayed.
 ****************************************************************************************************/

// Project specific includes
#include "msp.h"
#include "ST7735.h"
#include "LCD.h"
#include "SLOT_MACHINE.h"
#include "TIME.h"
#include "GPIO.h"

// standard includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// global variables
volatile bool spin = false;
volatile bool victory = false;

// define symbols
const char *symbols[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

// slot machine rows
char row1[4] = {' ', ' ', ' ', '\0'};
char row2[4] = {' ', ' ', ' ', '\0'};
char row3[4] = {' ', ' ', ' ', '\0'};

/*
 * main :   Main function that initializes the system and enters an infinite loop
 *          to run the slot machine.
 *
 * INPUTS:
 *      N/A
 *
 * OUTPUTS:
 *      N/A
 *
 * NOTES:
 *      This function sets up the watchdog timer, initializes GPIO pins, the SysTick timer, the LCD,
 *      and the port interrupt. The function continuously spins the slot machine reels given the
 *      flag is true.
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // initialize GPIOs
    set_clk48MHz();
    pin_init();
    sys_init();
    ST7735_InitR(INITR_REDTAB);

    // initialize interrupt
    NVIC_EnableIRQ(PORT4_IRQn);
    __enable_irq();

    //start playing the slot machine game
    start_screen();

    while (1){
        if(spin) {
            spin_reels();
            delay_ms(400);  //delay controls how fast the reels spin
        }
    }
}
