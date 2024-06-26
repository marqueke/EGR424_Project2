/*****************************************************************
 *  GPIO.c
 *
 *  Authors       Kelsey Marquez and Jacob Kucinski
 *  Course        EGR 424: Design of Microcontroller Applications
 *  Instructor    Dr. Parikh
 *  Assignment    Project 2: Slot Machine
 *  Date          6/27/24
 *****************************************************************/

// Project specific includes
#include "msp.h"
#include "GPIO.h"
#include "ST7735.h"
#include "TIME.h"
#include "SLOT_MACHINE.h"
#include "LCD.h"

// standard includes
#include <stdlib.h>
#include <stdbool.h>

extern volatile bool spin;
extern volatile bool victory;

/*-------------------------------------------------------------------------------//
 * Function      pin_init(void)
 * Description   Initializes GPIO pins to interface with external buttons
 *------------------------------------------------------------------------------*/
void pin_init(void){
    // DEFINE P4 AS GPIO
    P4->SEL1 &= ~BTN_MASK;
    P4->SEL0 &= ~BTN_MASK;
    P4->IES  |=  BTN_MASK;                  // set to detect falling edge
    P4->IE   |=  BTN_MASK;                  // enable port interrupt
    P4->IFG   =  0;                         // clr intpt flag

    // SET UP BUTTONS
    P4->DIR &= ~BTN_MASK;                   //Set buttons as input
    P4->REN |= BTN_MASK;                    //Enable Internal resistance
    P4->OUT |= BTN_MASK;                    //Set pull-up resistor

    NVIC->ISER[1] = 1 << ((PORT4_IRQn) & 31); // Enable Port 4 interrupt in NVIC
}

/*-------------------------------------------------------------------------------//
 * Function     PORT4_IRQHandler(void)
 * Description  Trigger P4 interrupts with external buttons, set flag for use in other functions
 *              Disable interrupt for button that triggered handler until it is debounced
 *------------------------------------------------------------------------------*/
void PORT4_IRQHandler(void) {
    if (P4->IFG & BIT7) {   // Check if Start button is pressed
        if (debounce(20, BIT7)) {  // Debounce the start button
            P4->IFG &= ~BIT7;   // Clear interrupt flag after confirming debounce

            if(victory) {
                // Reset the game to the start screen after a victory
                victory = false;
                start_screen();
            } else {
                //continue to play the game
                ST7735_FillScreen(ST7735_WHITE);
                spin = true;
            }
        }
    } else if (P4->IFG & BIT6) {  // Check if Stop button is pressed
        if (debounce(20, BIT6)) {  // Debounce the stop button
            P4->IFG &= ~BIT6;   // Clear interrupt flag after confirming debounce
            spin = false;       // Stop any ongoing spinning
        }
    }
}
