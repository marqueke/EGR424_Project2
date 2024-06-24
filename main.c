/*******************************************************************************************************
 *Author        Kelsey Marquez and Jacob Kucinski
 *Course        EGR 424: Design of Microcontroller Applications
 *Assignment    Project 2: Slot Machine
 *Date          6/27/24
 *Instructor    Dr. Parikh
 *Description   Interface MSP432 with 128x160 ST7735 LCD
 *Notes:        Includes modified Valvano graphics library for interfacing with the ST7735 LCD
 ****************************************************************************************************/
#include "msp.h"
#include <ST7735.h>
#include "images.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void pin_init(void);                        // GPIO init
void PORT4_IRQHandler(void);                // buttons interrupt handler
void set_clk48MHz(void);                    // clock init for LCD
void start_screen(void);                    // LCD display
void spin_reels(void);                      // LCD display / game function
void display_row(char row[], int line);     // LCD display
void check_victory(void);                   // LCD display / game function

//for jacob.
//L->R
//vcc, gnd, cs, reset, ao, sda, sck, led(no)
//white, yellow, blue, purple, orange, grey, green
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

uint8_t BTN_MASK = 0b11000000;      // mask for BTN Pins
uint8_t BTN1 = BIT6,                // define button 1 bit
        BTN2 = BIT7;                // define button 2 bit

volatile bool spin = false;

// Define symbols
const char *symbols[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

// slot machine rows
// added spaces between the numbers for easier display on LCD
char row1[6] = {' ', ' ', ' ',' ', ' ', '\0'};
char row2[6] = {' ', ' ', ' ',' ', ' ', '\0'};
char row3[6] = {' ', ' ', ' ',' ', ' ', '\0'};

/*-------------------------------------------------------------------------------//
 * Function     main()
 *-------------------------------------------------------------------------------*/
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    set_clk48MHz();
    pin_init();
    ST7735_InitR(INITR_REDTAB);
    NVIC_EnableIRQ(PORT4_IRQn);
    __enable_irq();

    start_screen();
    while (1){
        if(spin) {
            spin_reels();
            __delay_cycles(50000000);
        }
    }
}

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
    if (P4->IFG & BIT7) {   // Start button (P4.7) pressed
        P4->IFG &= ~BIT7;   // Clear interrupt flag
        ST7735_FillScreen(ST7735_WHITE);
        spin = true;        // Start spinning
    } else if (P4->IFG & BIT6) { // Stop button (P4.6) pressed
        P4->IFG &= ~BIT6;   // Clear interrupt flag
        spin = false;        // stop spinning
        check_victory();
    }

    //to add: third button to "cheat" the game and guarantee victory by placing 3 matching symbols (to show functionality)
}

/*-------------------------------------------------------------------------------//
 * Function:        set_clk48MHz(void)
 * Description:     *This function was posted by Dr. Kandalaft
 *                  This function sets up the MCLK for 48 MHz and the SMCLK for
 *                  12 MHz.
 *------------------------------------------------------------------------------*/
void set_clk48MHz(void)
{
    // Configure Flash wait-state to 1 for both banks 0 & 1
    // *Require 1 wait state for flash access when clk speed > 24MHz
       FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
       FLCTL_BANK0_RDCTL_WAIT_1;
       FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
       FLCTL_BANK1_RDCTL_WAIT_1;

    //Configure HFXT to use 48MHz crystal, source to MCLK & HSMCLK*
       PJ->SEL0 |= BIT2 | BIT3;                     // Configure PJ.2/3 for HFXT function
       PJ->SEL1 &= ~(BIT2 | BIT3);
       CS->KEY = CS_KEY_VAL ;                       // Unlock CS module for register access
       CS->CTL2 |= CS_CTL2_HFXT_EN |                // enable HFXT oscillator
                CS_CTL2_HFXTFREQ_6 |                // set freq to 40+MHz
                CS_CTL2_HFXTDRIVE;
          while(CS->IFG & CS_IFG_HFXTIFG)
                    CS->CLRIFG |= CS_CLRIFG_CLR_HFXTIFG;

    /* Select MCLK & HSMCLK = HFXT, no divider */
      CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK     |
                              CS_CTL1_DIVM_MASK     |
                              CS_CTL1_SELS_MASK     |
                              CS_CTL1_DIVHS_MASK)   |   // clr dividers
                              CS_CTL1_SELM__HFXTCLK |   // MCLK sourced from HFXTCLK
                              CS_CTL1_SELS__HFXTCLK;    // SMCLK & HSMCLK sourced from HFXTCLK

      CS->CTL1 = CS->CTL1 |CS_CTL1_DIVS_2;    // change the SMCLK clock speed to 12 MHz.

      CS->KEY = 0;                            // Lock CS module from unintended accesses
}

/*-------------------------------------------------------------------------------//
 * Function:        start_screen(void)
 * Description:     Welcome screen to display that prompts the user to begin
 *                  playing the slot machine
 *------------------------------------------------------------------------------*/
void start_screen(void)
{
    char start_txt[5][17]= {"Welcome to", "the slot", "machine!", "Spin to", "play!"};
    ST7735_FillScreen(ST7735_WHITE);
    ST7735_DrawString(1, 2, start_txt[0], ST7735_BLACK);
    ST7735_DrawString(3, 4, start_txt[1], ST7735_BLACK);
    ST7735_DrawString(3, 6, start_txt[2], ST7735_BLACK);
    ST7735_DrawString(3, 10, start_txt[3], ST7735_BLACK);
    ST7735_DrawString(6, 12, start_txt[4], ST7735_BLACK);
}

/*
 * eliminated shift_rows function, and condensing into spin_reels()
 * to solve the issue of repeated numbers on lines 2 and 3
 */
/*-------------------------------------------------------------------------------//
 * Function:        spin_reels(void)
 * Description:     Function that handles the movement and generation of the
 *                  slot machine symbols
 *------------------------------------------------------------------------------*/
void spin_reels(void) {
    int i = 0;

    // shift rows upwards. row2->row1, row3->row2
    for(i=0; i < 5; i++) {
        row1[i] = row2[i];
        row2[i] = row3[i];
        i++;
    }

    //make new third row
    for(i = 0; i < 5; i++) {
        row3[i] = symbols[rand() % 9][0];
        i++;
    }

    //display new rows after shift and random generated third row
    display_row(row1, 1);   // display top row
    display_row(row2, 2);   // display middle row
    display_row(row3, 3);   // display bottom row
}

/*-------------------------------------------------------------------------------//
 * Function:        display_row(char row[], int line)
 * Description:     Function used by spin_reels() that will display the slot
 *                  machine rows on the LCD
 *------------------------------------------------------------------------------*/
void display_row(char row[], int line) {
    ST7735_DrawString(6, line * 4, row, ST7735_BLACK);
}

/*-------------------------------------------------------------------------------//
 * Function:        check_victory(void)
 * Description:     Function to check if the user has won on the slot machine.
 *                  Victory is achieved by matching 3 alike symbols in any row
 *------------------------------------------------------------------------------*/
void check_victory(void) {

    char victory_text[4][17]= {"Victory!", "You WIN!", "Spin", "again!"};

    // Check if any row has 3 of the same symbols
    if ((row1[0] == row1[2] || row1[2] == row1[4]) ||
        (row2[0] == row2[2] || row2[2] == row2[4]) ||
        (row3[0] == row3[2] || row3[2] == row3[4])) {

        ST7735_FillScreen(ST7735_WHITE);
        ST7735_DrawString(3, 2, victory_text[0], ST7735_BLACK);
        ST7735_DrawString(3, 4, victory_text[1], ST7735_BLACK);
        ST7735_DrawString(6, 10, victory_text[2], ST7735_BLACK);
        ST7735_DrawString(5, 12, victory_text[3], ST7735_BLACK);
        spin = false;
    }

    if (row1[0] == row1[2] || row1[2] == row1[4]){
        ST7735_DrawString(6, 7, row1, ST7735_BLACK);
    }
    else if(row2[0] == row2[2] || row2[2] == row2[4]){
        ST7735_DrawString(6, 7, row2, ST7735_BLACK);
    }
    else if(row3[0] == row3[2] || row3[2] == row3[4]){
        ST7735_DrawString(6, 7, row3, ST7735_BLACK);
    }
}
