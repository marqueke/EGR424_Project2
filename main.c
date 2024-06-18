/*******************************************************************************************************
 *Author        Kelsey Marquez
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

void pin_init(void);
void PORT4_IRQHandler(void);
void check_victory(void);
void set_clk48MHz(void);
void start_screen();
void shift_rows(void);
void spin_reels(void);
void display_row(char row[], int line);

/**********PIN MAP************
 * * * * * * * * * * * * * * *
 ******** ST7735 LCD  ********
 * CLK                   >P9.5
 * SDA                   >P9.7
 * RS (A0)               >P9.2
 * RST                   >P9.3
 * CS                    >P9.4
 ********** BUTTONS **********
 * STOP                  >P4.6
 * START                 >P4.7
******************************/

uint8_t BTN_MASK = 0b11000000;      // mask for BTN Pins
uint8_t BTN1 = BIT6,                // define button 1 bit
        BTN2 = BIT7;                // define button 2 bit

volatile bool spin = false;
//volatile bool stop = true;

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
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    pin_init();
    ST7735_InitR(INITR_REDTAB);

    NVIC_EnableIRQ(PORT4_IRQn);
    __enable_irq();

    ST7735_FillScreen(ST7735_WHITE);

    while (1){
        if(spin) {
            spin_reels();
            __delay_cycles(5000000);
        }
    }
}

/*-------------------------------------------------------------------------------//
 * Function      pin_init()
 * Description   Initializes GPIO pins to interface with external buttons and LEDs
 *------------------------------------------------------------------------------*/
void pin_init(){
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
 * Function     PORT4_IRQHandler()
 * Description  Trigger P4 interrupts with external buttons, set flag for use in other functions
 *              Disable interrupt for button that triggered handler until it is debounced
 *------------------------------------------------------------------------------*/
void PORT4_IRQHandler(void) {
    if (P4->IFG & BIT7) {   // Start button (P4.7) pressed
        P4->IFG &= ~BIT7;   // Clear interrupt flag
        spin = true;        // Start spinning
    } else if (P4->IFG & BIT6) { // Stop button (P4.6) pressed
        P4->IFG &= ~BIT6;   // Clear interrupt flag
        spin = false;        // stop spinning
    }
}

void display_row(char row[], int line) {
    ST7735_DrawString(0, line * 4, row, ST7735_BLACK);
}

// check if there are 3 symbols in a row
void check_victory(void) {
    // Check if any row has 3 of the same symbols
    if ((row1[0] == row1[1] && row1[1] == row1[2]) ||
        (row2[0] == row2[1] && row2[1] == row2[2]) ||
        (row3[0] == row3[1] && row3[1] == row3[2])) {
            ST7735_DrawString(100, 100, "Victory!", ST7735_BLACK);
    }
}

void spin_reels(void) {
    int i = 0;

    for(i = 0; i < 3; i++) {
        row3[i] = symbols[rand() % 9][0];
    }
    row3[3] = '\0';         // null-terminate string
    display_row(row3, 3);   // display on bottom row
    shift_rows();
}

void shift_rows(void) {
    int i = 0;

    // shift row upwards
    for(i=0; i < 3; i++) {
        row1[i] = row2[i];
        row2[i] = row3[i];
    }
    row1[3] = '\0';
    row2[3] = '\0';
    display_row(row2, 2);   // display in middle row
    display_row(row1, 1);   // display in top row
    check_victory();
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
 * Function:        start_screen()
 * Description:     Set image on screen
 *                  * when setting rotation, must also set parameters of draw
 *                  function to match the 'new' dimensions
 *
 *------------------------------------------------------------------------------*/
void start_screen()
{
    char start_txt[2][17]= {"EMBEDDED", "SYSTEMS"};
    ST7735_SetRotation(1);                              // rotate display 90*
//    ST7735_DrawBitmap(0, 128, triangle, 160, 128);        // set image on screen
    ST7735_DrawString(6, 4, start_txt[0], ST7735_BLACK);// print 1st string
    ST7735_DrawString(6, 6, start_txt[1], ST7735_BLACK);// print 2nd string, set
                                                        // with 1 row space btwn 1st
    Delay1ms(3000);                                     // delay 3000ms
}

