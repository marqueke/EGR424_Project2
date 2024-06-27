/*******************************************************************************************************
 *Authors       Kelsey Marquez and Jacob Kucinski
 *Course        EGR 424: Design of Microcontroller Applications
 *Assignment    Project 2: Slot Machine
 *Date          6/27/24
 *Instructor    Dr. Parikh
 *Description   Interface MSP432 with 128x160 ST7735 LCD
 *Notes:        Includes modified Valvano graphics library for interfacing with the ST7735 LCD
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

volatile bool spin = false;
volatile bool victory = false;

// Define symbols
const char *symbols[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

// slot machine rows
char row1[4] = {' ', ' ', ' ', '\0'};
char row2[4] = {' ', ' ', ' ', '\0'};
char row3[4] = {' ', ' ', ' ', '\0'};

/*-------------------------------------------------------------------------------//
 * Function     main()
 *-------------------------------------------------------------------------------*/
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    set_clk48MHz();
    pin_init();
    sys_init();
    ST7735_InitR(INITR_REDTAB);
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
